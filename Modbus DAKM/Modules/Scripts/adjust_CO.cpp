//---------------------------------------------------------------------------------------------------------------------------------------
//���� ���������� CO2
namespace adjust_CO
{	
string caption = "������������� CO";

//���������� ����������������  C�
void RunScript_adjust_gas()
{	
	adjust_gas.GAS_NUMBER = Gas.CO_3;		
	adjust_gas.CMD_CODE = Cmd.adjust_CO;
	adjust_gas.CONC_VAR = Cco;	
};

};// namespace adjust_0