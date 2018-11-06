//---------------------------------------------------------------------------------------------------------------------------------------

namespace adjust_all
{	
string caption = "Калибровка каналов измерения";
//Калибровка нуля шкалы
void RunScript_adjust_0(){};
//Калибровка чувствительности  CH
void CH_RunScript_adjust_gas()
{
	adjust_gas.GAS_NUMBER = Gas.CH_3;		
	adjust_gas.CMD_CODE = Cmd.adjust_CH;			
	adjust_gas.CONC_VAR = Cch;	
};
//Калибровка чувствительности  CО
void CO_RunScript_adjust_gas()
{	
	adjust_gas.GAS_NUMBER = Gas.CO_3;		
	adjust_gas.CMD_CODE = Cmd.adjust_CO;
	adjust_gas.CONC_VAR = Cco;	
};
//Калибровка чувствительности  CO2
void CO2_RunScript_adjust_gas()
{	
	adjust_gas.GAS_NUMBER = Gas.CO2_3;		
	adjust_gas.CMD_CODE = Cmd.adjust_CO2;			
	adjust_gas.CONC_VAR = Cco2;
};

};