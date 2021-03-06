//------------------------------------------------------------------------------
#ifndef toolsH
#define toolsH
//------------------------------------------------------------------------------
#include "MyExcpt.hpp"
#include <vector>
#include <map>

#include "vardef.hpp"
#include "..\my_include\free_types_.h"


namespace DAK
{
    void InitializeConc();
    struct ConcT
    {
        double value;
        AnsiString sConc;
        double failureCode;
        bool ok;
    };
    ConcT GetModbusConc(unsigned addy, bool enableLog = true);


    struct DevState
    {
        double current;
        bool rele[RelePin::count];
        AnsiString sIout, sConcI, sPorog1, sPorog2;
        double failureCode;
        bool ok;
    };

    DevState GetDevState(unsigned addy, bool enableLog = true);

    void WriteKef(unsigned addy, unsigned kNum);
    Maybe<double> ReadKef(unsigned addy, unsigned kNum);

    Maybe<double> ReadVar1(unsigned addy, bool enableLog);
    Maybe<double> ReadT(unsigned addy, bool enableLog);
    Maybe<double> ReadIlampWorkkRefk (unsigned addy, bool enableLog);
    Maybe<double> ReadIlampOn (unsigned addy, bool enableLog);
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
