//------------------------------------------------------------------------------
#include <vector>
#include <algorithm>
#include <numeric>
#pragma hdrstop
#include "HeatChamber.h"
#include "AnsiStringUtils_.h"
#include "guicon.h"
#include "my_array_size.h"
#include "ctrlsys.h"
#include "MasterSlaveIO.h"
#include "MyExcpt.hpp"
#include "Unit1.h"
#include "boost\regex.hpp"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
typedef std::vector<unsigned char> VInt8;
//------------------------------------------------------------------------------
namespace HeatCham
{
//------------------------------------------------------------------------------
unsigned HexToAsci( const char* beg, const char* end )
{
	AnsiString s = "0x";
	for( ; beg!=end; ++beg )
	{
		const char ch = *beg;
		s = s + AnsiString(ch);
	}
	return StrToIntDef(s,0);
}
//------------------------------------------------------------------------------
VInt8 StrToAsciInt(const AnsiString &str)
{
    VInt8 ret;
	for( unsigned i=1; (int)i<str.Length() + 1; ++i ){
		ret.push_back( str[i] );
	}
	return ret;
}
//------------------------------------------------------------------------------
VInt8 MakeRequest(const VInt8& dat)
{
	VInt8 res(1,2);
	res.insert( res.end(), dat.begin(), dat.end() );
	const unsigned char sum  = std::accumulate(res.begin()+1,res.end(),0);

	AnsiString sumStr = IntToHex( sum, 2 );
	const char s1 = sumStr[1], s2 = sumStr[2];
	res.push_back(0x0D);
	res.push_back(0x0A);
	return res;
}
//------------------------------------------------------------------------------
VInt8 MakeRequest(const AnsiString& cmd)
{
	return MakeRequest( StrToAsciInt(cmd) );
}
//------------------------------------------------------------------------------
bool SuccessedWRDCommand(const char* rxd, unsigned len)
{
    const unsigned char goodAnswerOnWRDCommand[] =
	    {0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x4F, 0x4B, 0x0D, 0x0A};

    return len==array_size(goodAnswerOnWRDCommand) &&
	    std::equal( rxd, rxd+len, goodAnswerOnWRDCommand );
}
//------------------------------------------------------------------------------
bool SuccsecedGetTCommand( const char* rxd, unsigned len, double& t, double& setpoint, AnsiString& error )
{
    std::string::const_iterator xItStart = rxd, xItEnd = rxd + len, xIt = xItStart;
    boost::cmatch xResults;
    const char re[] = "01RRD,OK,([0-9a-fA-F]{4,4}),([0-9a-fA-F]{4,4})\r\n$";
    if( !boost::regex_search(xIt, xItEnd, xResults, boost::regex(re) ) )
    {
        error = "не соответствует образцу";
        return false;
    }

    if (xResults.size() < 3)
    {
        error = "менее двух групп";
        return false;
    }
    std::string str;

    str = std::string(xResults[1].first, xResults[1].second);
    t = myHexToInt(str.c_str()) / 10.0 ;

    str = std::string(xResults[2].first, xResults[2].second);
    setpoint = myHexToInt(str.c_str()) / 10.0;

    return true;
}
//------------------------------------------------------------------------------
namespace Cmd
{
    const AnsiString
		getT 		    = "01RRD,02,0001,0002", 	// 	"Запрос текущей темературы камеры"

		setFxMd 	    = "01WRD,01,0104,0001",		// "Перевод камеры в режим фиксированной работы

		setStart800	    = "01WRD,01,0101,0001",		// "Старт"
		setStop800      = "01WRD,01,0101,0004",		// "стоп"
		// Задание уставки val в режиме фиксированной работы
		setT800         = "01WRD,01,0102,",

		setStart2500	= "01WRD,01,0102,0001",		// "Старт"
		setStop2500     = "01WRD,01,0102,0004",		// "стоп"
		// Задание уставки val в режиме фиксированной работы
		setT2500        = "01WRD,01,0104," ;
};
//------------------------------------------------------------------------------
MasterSlaveIOImpl& IO()
{
    return *CtrlSys().Instance().GetIOSets().heatCham;
}

AnsiString TermoType()
{
    return CtrlSys().Instance().GetIOSets().termoType;
}

bool IsTermoType800()
{
    return TermoType() == AnsiString("800");
}
//------------------------------------------------------------------------------
void SendControlRequest(const AnsiString &cmd, const AnsiString &msg)
{
    const VInt8 req = MakeRequest(cmd);
    MYCOUT_("Управление термокамерой %s: %s, %s\n", TermoType(), msg, cmd ));
    IO().Send( req.begin(), req.end(), true );
    if(  !SuccessedWRDCommand( IO().RxD(), IO().RxDSize() ) )
        MY_THROW_("Ошибка термокамеры!");
}
//------------------------------------------------------------------------------
void SetSetpoint(int t)
{
    //const AnsiString sT = t>=0 ? IntToHex( t,2) : "-"+IntToHex( std::abs(t),2);
    //const AnsiString sT = IntToHex( t,4);

    //const AnsiString sT = t>=0 ? IntToHex( t,4) : "-"+IntToHex( std::abs(t),4);
    /*
    if (t>=0) {
        SendControlRequest( Cmd::setT + IntToHex( t,4), MYSPRINTF_("уставка %g", t/10.0) );
    } else {
        SendControlRequest( Cmd::setT + "-"+IntToHex( std::abs(t),4), MYSPRINTF_("уставка %g", t/10.0) );
    }
    */
    std::string s = AnsiString::IntToHex( t,4).c_str();
    while(s.size()<4)
        s = "00" + s;
    while (s.size()>4)   {
        std::rotate(s.begin(), s.begin() + 1, s.end());
        s = s.substr(0, s.size()-1);
    }

    AnsiString setT = Cmd::setT800;
    if (!IsTermoType800()){
        setT = Cmd::setT2500;
    }

    SendControlRequest( setT + s.c_str(), MYSPRINTF_("уставка %g", t/10.0) );


    Form1->btnHeatchamSetpoint->Caption = t/10.0;
    Form1->AddLogAll( MYSPRINTF_("Уставка темокамеры %g", t/10.0));
}
void Start()
{
    AnsiString setStart = Cmd::setStart800;
    if (!IsTermoType800()){
        setStart = Cmd::setStart2500;
    }
    SendControlRequest( setStart, "старт внешнего управления" );
}
void Stop()
{
    AnsiString setStop = Cmd::setStop800;
    if (!IsTermoType800()){
        setStop = Cmd::setStop2500;
    }
    SendControlRequest( setStop, "остановка внешнего управления" );
}
void Fix()
{
    SendControlRequest( Cmd::setFxMd, "перевод камеры в режим фиксированной работы" );
}
//------------------------------------------------------------------------------

double GetTemperature1()
{
    const VInt8 req = MakeRequest( Cmd::getT );
    MYCOUT_("Запрос температуры темокамеры %s\n", Cmd::getT ));
    IO().Send( req.begin(), req.end(), true );
    const char *rxd = IO().RxD();
    MYCOUT_("Термокамера: \"%s\"\n", AnsiString(rxd, IO().RxDSize() ) ));

    double t,setpoint;
    AnsiString error;

    if(  !SuccsecedGetTCommand( rxd, IO().RxDSize(), t, setpoint, error ) ) {
        MY_THROW_( AnsiString("Ошибка термокамеры. ") + error);
    }

    return t;
}
double GetTemperature()
{
    Form1->btnHeatchamT->Font->Color = clGreen;
    Form1->btnHeatchamT->Caption = "...";

    try
    {
        const double ret = GetTemperature1();
        Form1->btnHeatchamT->Caption = ret;
        return ret;
    }
    catch(...)
    {
        Form1->btnHeatchamT->Font->Color = clRed;
        Form1->btnHeatchamT->Caption ="ошибка";
        throw;
    }
}

//------------------------------------------------------------------------------
};  // namespace HeatCham
//------------------------------------------------------------------------------
