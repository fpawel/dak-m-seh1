//---------------------------------------------------------------------------------------------------------------------------------------
//Цикл калибровки CO2
namespace adjust_CO2
{	
string caption = "Корректировка CO2";

//Калибровка чувствительности  CO2
void RunScript_adjust_gas()
{	
	adjust_gas.GAS_NUMBER = Gas.CO2_3;		
	adjust_gas.CMD_CODE = Cmd.adjust_CO2;			
	adjust_gas.CONC_VAR = Cco2;
};

};// namespace adjust_0