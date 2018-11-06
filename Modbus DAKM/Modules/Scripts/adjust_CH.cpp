//---------------------------------------------------------------------------------------------------------------------------------------
//Цикл калибровки CH
namespace adjust_CH
{	
string caption = "Корректировка CH";
//Калибровка чувствительности  CH
void RunScript_adjust_gas()
{
	adjust_gas.GAS_NUMBER = Gas.CH_3;		
	adjust_gas.CMD_CODE = Cmd.adjust_CH;			
	adjust_gas.CONC_VAR = Cch;	
};

};// namespace adjust_0