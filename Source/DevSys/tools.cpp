//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "tools.h"
//------------------------------------------------------------------------------
#include <algorithm>
#include <math>
//------------------------------------------------------------------------------
#include "boost\assign\list_of.hpp"
//------------------------------------------------------------------------------
#include "ctrlsys.h"
#include "myconv.h"
#include "AnsiStringUtils_.h"
#include "guicon.h"
#include "Unit1.h"
#include "MyVclUtils_.h"
#include "bin_nums.h"
#include "TransferManage.h"
#include "..\my_include\MyNoDestroySingleton.hpp"
#include "MasterSlaveIO.h"
#include "Myformat.hpp"
#include "modbus_.h"
#include "bin_nums.h"
#include "ModbusAdapter.h"
#include "vardef.hpp"
#include "pneumo.hpp"
#include "FooCalledOnce.hpp"
#include "Devices.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
namespace DAK
{
ModbusAdapter& Modbus()
{
    return *CtrlSys().Instance().GetIOSets().modbus;
}
//------------------------------------------------------------------------------
void LogValue(unsigned addy, const AnsiString &s, const AnsiString &what, unsigned nVar, bool enableLog)
{
    Vars::Set(addy, nVar, s);
    if(enableLog) {
        const ConsoleScreenAtrributesHolder cah(MY_SET_CONSOLE_VELVET_TEXT);
        Form1->AddAddyLog( addy, s);
    }
}
//------------------------------------------------------------------------------
ConcT  GetModbusConc(unsigned addy, bool enableLog)
{
    const ConsoleScreenAtrributesHolder cah(MY_SET_CONSOLE_YELLOW_TEXT);
    ConcT ret;

    const ModbusValue1T v = ReadModbusValue1(  Modbus(), addy, 0 );
    ret.value = v.conc;
    ret.ok = v.ok;
    //ret.conc = 1.1+random(10)/10.0;
    if (!ret.ok) {
        return ret;
    }

    Maybe<double> vFailure = ReadModbusFloat(  Modbus(), addy, 88 );
    ret.ok = vFailure.ok;
    ret.failureCode = vFailure.value;
    if (!vFailure.ok) {
        Form1->LogAddyError(addy, "не удалось считать регистр 88 (отказ)");
    }

    ret.sConc = Format::Conc(ret.value, ret.failureCode);
    LogValue(addy, ret.sConc, "конц.", Vars::C, enableLog);
    return ret;
}
//------------------------------------------------------------------------------
DevState GetDevState(unsigned addy, bool enableLog)
{
    const ConsoleScreenAtrributesHolder cah(MY_SET_CONSOLE_YELLOW_TEXT);

    CtrlSysImpl& ctrlSys = CtrlSys::Instance();
    DevState ret;
    ret.ok = false;

    Maybe<double> vFailure = ReadModbusFloat(  Modbus(), addy, 88 );
    ret.failureCode = vFailure.value;
    if (!vFailure.ok) {
        Form1->LogAddyError(addy, "не удалось считать регистр 88 (отказ)");
        return ret;
    }

    ReadModbusRegistersData(  Modbus(), ctrlSys.GetIOSets().stendAddy, (addy-1)*2, 2 );
    const unsigned char *dt = Modbus().AcceptedData();

    const int
        acceptedDataSize = Modbus().AcceptedDataSize();


    if( Modbus().AcceptedDataSize()!=5 ){
        Form1->LogAddyError(addy, "ошибка протокола стенда: ожидалось 5 байт ответа");
        return ret;
    }

    ret.current = 1.0*( dt[1]*256.0+dt[2])/100.0;
    const bool p1 = ( Getbit(dt[4], 1)==0 ), p2 = ( Getbit(dt[4], 2)==0 );

    //const bool p1 = 1, p2 = 1;
    //ret.current = 4+random(16)/16.0;

    ret.sIout = Format::Iout(ret.current, ret.failureCode);
    ret.rele[RelePin::rele1] = p1;
    ret.rele[RelePin::rele2] = p2;

    LogValue(addy, ret.sIout,   "Iвых", Vars::Iout, enableLog);

    const double conc_i = I2Conc(ret.current);
    ret.sConcI = Format::Conc(conc_i, ret.failureCode);
    LogValue(addy, ret.sConcI,  "конц.ток", Vars::C_Iout, enableLog);

    ret.sPorog1 = Format::Porog1(p1),
    ret.sPorog2 = Format::Porog2(p2);

    const DAK::Sets sets = DAK::Sets::Get();
    const unsigned isp = StrToIntDef(sets.ispolnenie,0);
    const unsigned skipIsp[8] = { 23, 24, 25, 31, 32, 33, 131, 132};
    if(std::find(skipIsp, skipIsp+5, isp)==skipIsp+5) {
        LogValue(addy, ret.sPorog1, "Порог", Vars::rele1, enableLog);
        LogValue(addy, ret.sPorog2, "Порог", Vars::rele2, enableLog); }

    ret.ok = true;
    return ret;
}

void WriteKef(unsigned addy, unsigned kNum)
{
    const DAK::Kefs kefs = DAK::GetKefs();
    const DAK::Kef kef = kefs[kNum];
    const unsigned cmd = ( 0x80 << 8 ) + kef.n;
    const AnsiString sVal = Devs::GetKefOfAddy(addy, kNum);

    AnsiString s;
    s.sprintf("записать кэф.%d %s=%s", kef.n, kefs[kNum].caption, sVal);
    double val;
    if( TryStr2Float( sVal.c_str(), &val ) )
        WriteModbusFloat( Modbus(), addy, cmd, val );
    else
        s += ", некорректное значение";
    Form1->AddAddyLog(addy, s );
}

Maybe<double> ReadKef(unsigned addy, unsigned kNum)
{
    const DAK::Kefs kefs = DAK::GetKefs();
    const DAK::Kef kef = kefs[kNum];

    Maybe<double> v = ReadModbusFloat( Modbus(), addy, 224 + kef.n*2 );

    if (v.ok) {
        Devs::SetKefOfAddy(addy, kNum, v.value);
        AnsiString s;
        s.sprintf("считан кэф.%d %s=%g", kef.n, kefs[kNum].caption, v.value);
        Form1->AddAddyLog(addy, s );
    }

    return v;
}

Maybe<double> ReadVar1(unsigned addy, bool enableLog)
{
    Maybe<double> val = ReadModbusFloat( Modbus(), addy, 16 );
    LogValue(addy, val.value, "var1", Vars::var1, enableLog);
    return val;
}

// ток излучателя
Maybe<double> ReadIlampWorkkRefk (unsigned addy, bool enableLog)
{
        AnsiString s;
    Maybe<double> val = ReadModbusFloat( Modbus(), addy, 4 );
    if(enableLog) {
        s.sprintf("Ilamp=%g", val.value);
        Form1->AddAddyLog( addy, s);
    }

    val = ReadModbusFloat( Modbus(), addy, 14 );
    if(enableLog) {
        s.sprintf("Workk=%g", val.value);
        Form1->AddAddyLog( addy, s);
    }

    val = ReadModbusFloat( Modbus(), addy, 12 );
    if(enableLog) {
        s.sprintf("Refk=%g", val.value);
        Form1->AddAddyLog( addy, s);
    }
    return val;
}

Maybe<double> ReadIlampOn (unsigned addy, bool enableLog){
    AnsiString s;
    Maybe<double> val = ReadModbusFloat( Modbus(), addy, 4 );
    if(enableLog) {
        s.sprintf("Ilamp=%g", val.value);
        Form1->AddAddyLog( addy, s);
    }
    
    return val;

}

// датчик температуры
Maybe<double> ReadT(unsigned addy, bool enableLog)
{
    const Maybe<double> val = ReadModbusFloat( Modbus(), addy, 2 );
    LogValue(addy, val.value, "T\"C", Vars::Tpp, enableLog);
    return val;
}


//------------------------------------------------------------------------------
};// namespace Stend_STM30
//------------------------------------------------------------------------------
