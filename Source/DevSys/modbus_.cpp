#include <DateUtils.hpp>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "modbus_.h"
#include "bcd_.h"
#include "bin_nums.h"
#include "my_array_size.h"
#include "Loki\static_check.h"
#include "MyExcpt.hpp"
// для вызова функции PerformModbusTransfer
#include "AnsiStringUtils_.h"
#include "guicon.h"
#include "ModbusAdapter.h"
#include "MyExcpt.hpp"




// предварительные объявления классов-исключений для ХРОМАТ
// #include "HromatExceptLst.hpp"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
enum{ READ_MODBUS_FILE_CMD_CODE = 0x41 };
enum{ WRITE_MODBUS_FILE_CMD_CODE = 0x51 };

OnReadModbusFloatValue onReadModbusFloatValue;

void SetOnReadModbusFloatValue(OnReadModbusFloatValue f){
    onReadModbusFloatValue = f;
}



namespace PrivateHromatModbus
{

	typedef struct{ unsigned char aa[3]; } AStruct;
    struct StaticCheckThisModule
    {
        void Check()
        {
        	//LOKI_STATIC_CHECK(sizeof(AStruct)==3, MSG);
            //STATIC_CHECK( sizeof(ModbusFileInfo)==MODBUS_FILE_INFO_SIZE+1, ModbusFileInfoSizeMismatch);
        }
    };
};


AnsiString BCDBuffToStr(const unsigned char* dt)
{
	return MYSPRINTF_( "%s, %s, %s, %s", IntToHex(dt[0],2),	IntToHex(dt[1],2),
    	IntToHex(dt[2],2),	IntToHex(dt[3],2) );
}



//------------------------------------------------------------------------------

void printError(AnsiString s)
{
   MyWCout( s, MY_SET_CONSOLE_RED_TEXT);
}

void printlnError(AnsiString s)
{
   printError( s + "\n");
}

void WriteModbus4Bytes( ModbusAdapter& protocol, const unsigned slave, unsigned cmd, const unsigned char* dt )
{
	//const AnsiString msg1 = FormatDeviceCommand(cmd, dt), msg = MYSPRINTF_( "%d:%s\n", slave, msg1 );
    //MyWCout(msg);
	enum
    {
    	MODUS_WRITE_REGISTER_COMMAND_CODE = 0x16,
        MODUS_WRITE_REGISTER_ADDR = 32,
        MODUS_WRITE_REGISTERS_COUNT = 3,
        MODUS_WRITE_DATA_LEN = 6,
        MODUS_WRITE_STR_LEN = 11
    };
	unsigned char txd[MODUS_WRITE_STR_LEN];
    txd[0] = MODUS_WRITE_REGISTER_ADDR >> 8;
    txd[1] = MODUS_WRITE_REGISTER_ADDR;
    txd[2] = MODUS_WRITE_REGISTERS_COUNT>>8;
    txd[3] = MODUS_WRITE_REGISTERS_COUNT;
    txd[4] = MODUS_WRITE_DATA_LEN;
    // данные
    txd[5] = cmd >> 8;
    txd[6] = cmd;
    std::copy( dt, dt+4, txd+7 );

    protocol.PerformTransfer( slave, MODUS_WRITE_REGISTER_COMMAND_CODE, txd,
        txd+MODUS_WRITE_STR_LEN);
    if(slave==0) return;
    if( (protocol.AcceptedDataSize()!=4) || !std::equal(txd, txd+4, protocol.AcceptedData()) )
        printError( AnsiString().sprintf("%d: Комманда 16: несоответсвие формата ответа\n",slave) );

}
//------------------------------------------------------------------------------
void WriteModbusFloat( ModbusAdapter& protocol, unsigned slave, unsigned cmd, double val )
{
	unsigned char bcd[4];
    FloatToBCD(val, bcd );
    MYCOUT_("%d: команда %s, %g\n", slave, IntToHex((int)cmd,2), val ), MY_SET_CONSOLE_AQUA_TEXT);
    WriteModbus4Bytes( protocol, slave, cmd, bcd);

}
void WriteBPS21M( ModbusAdapter& protocol, const unsigned slave, const unsigned nDevice,
    const unsigned cmdCode, bool high1_low0, double val )
{
	unsigned char bcd[4];
    FloatToBCD(val, bcd );
    Setbit( bcd[0], high1_low0, 3);
    const unsigned cmd = (nDevice<<8)+ cmdCode;
    WriteModbus4Bytes( protocol, slave, cmd, bcd);
}
//------------------------------------------------------------------------------

void ReadModbusRegistersData(  ModbusAdapter& protocol, unsigned slave, unsigned regAddr, unsigned regCount )
{
    assert( regCount>0 && regCount<0xFF );

    enum { MODUS_READ_REGISTER_COMMAND_CODE = 0x03 };
    const unsigned waitStrLen = regCount*2;
    
	unsigned char txd[4];
    txd[0] = regAddr >> 8;
    txd[1] = regAddr;
    txd[2] = regCount>>8;
    txd[3] = regCount;
    
    protocol.PerformTransfer( slave, MODUS_READ_REGISTER_COMMAND_CODE, txd, txd+4);
    const unsigned acceptedDataSize = protocol.AcceptedDataSize();
    if( acceptedDataSize!=waitStrLen + 1 ) {
        printError( AnsiString().sprintf("%d: Команда 3: несоответсвие формата ответа\n",slave) );
    }
}
//------------------------------------------------------------------------------
void printlnException(const FileLine& location)
{
    PMyExcpt e = RethrowMyException(location);
    if (e.get()!= NULL){
        printlnError("exception: " + e->ToStringAll());
    }
}

ModbusValue1T ReadModbusValue1(  ModbusAdapter& protocol, unsigned addy, unsigned regAddr )
{
    assert (onReadModbusFloatValue != NULL);
    ModbusValue1T ret;
    try
    {
        ReadModbusRegistersData( protocol, addy, regAddr, 2);
        const unsigned char *rxd = protocol.AcceptedData()+1;
        unsigned char dt[4];
        std::copy(rxd, rxd+4, dt );
        ret.porog1 = Getbit( dt[0], 3);
        ret.porog2 = Getbit( dt[0], 4);
        dt[0] &= 0x87;
        ret.conc = BCDToFloat( dt );
        ret.ok = true;
        onReadModbusFloatValue(addy, regAddr, ret.conc);
    }
    catch(...)
    {
        printlnException(__FILE_LINE__);
        ret.ok = false;
    }
    return ret;
}
//------------------------------------------------------------------------------
Maybe<double> ReadModbusFloat(  ModbusAdapter& protocol, const unsigned slave, unsigned regAddr )
{
    Maybe<double> result;
    result.ok = false;
    result.value = 0;

    ReadModbusRegistersData( protocol, slave, regAddr, 2);
    if ( protocol.AcceptedDataSize() < 5 ) {
        return result;
    }
    assert (onReadModbusFloatValue != NULL);
    unsigned char dt[4];
    try
    {
        const unsigned char *rxd = protocol.AcceptedData()+1;
        std::copy(rxd, rxd+4, dt );
        dt[0] &= 0x87;
    }
    catch (...)
    {
        printlnException(__FILE_LINE__);
        return result;
    }

    try
    {
        result.value = BCDToFloat( dt );
    }
    catch (...)
    {
        printlnException(__FILE_LINE__);
        return result;
    }
    try
    {
        onReadModbusFloatValue(slave, regAddr, result.value);
    }
    catch (...)
    {
        printlnException(__FILE_LINE__);
    }
    result.ok = true;
    return result;
}
//------------------------------------------------------------------------------
Maybe<unsigned> ReadModbusUnsigned(ModbusAdapter&  protocol, unsigned addy, unsigned regNum)
{
    Maybe<unsigned> result;
    result.ok = false;

    try
    {
        ReadModbusRegistersData(  protocol, addy, regNum, 1 );
        const unsigned char *rcv = protocol.AcceptedData()+1;
        result.value = rcv[0]*256 + rcv[1];
        result.ok = true;

    }
    catch (...)
    {
        printlnException(__FILE_LINE__);
        result.ok = false;
    }
    return result;

}
//------------------------------------------------------------------------------
