//---------------------------------------------------------------------------------------------------------------------------------------
namespace Private
{
	void FixPGSAndCalc(uint addy, uint grp, string name, uint n, variant gas)
	{		
		for( uint i=0; i<n; ++i )		
			Grp.SetValue(  addy, grp, name, i, gas[i]  )  ;
		Grp.CalcKoefs( addy, grp, n  ) ;	
	}
};


//���� "������������ �-�� ����������. �. CH"
namespace Lin
{	
string caption = "������������ ������� ������. ���. �������.";

//����������� ����� 1 CH
void CH_pt1_RunScript_BlowAndFix()		
{		
	using BlowAndFix; gas = Gas.N2; pt = 0; grp = Grp.LIN_CH;
};
//����������� ����� 2 CH
void CH_pt2_RunScript_BlowAndFix()		
{	
	using BlowAndFix; gas = Gas.CH_2; pt = 1;
};
//����������� ����� 3 CH
void CH_pt3_RunScript_BlowAndFix()		
{	
	using BlowAndFix; gas = Gas.CH_3; pt = 2;
};
//��������� � �������� �-�� ������ LIN_CH
void CalcAndWriteKoefs_LIN_CH(uint addy)
{		
	using Gas;
	Private.FixPGSAndCalc(addy, Grp.LIN_CH, "PGSch", 3, [N2, CH_2, CH_3] );
};

//����������� ����� 1 CO
void CO_pt1_RunScript_BlowAndFix()		
{	
	using BlowAndFix; gas = Gas.N2; pt = 0; grp = Grp.LIN_CO;
};
//����������� ����� 2 CO
void CO_pt2_RunScript_BlowAndFix()		
{	
	using BlowAndFix; gas = Gas.CO_2; pt = 1;
};
//����������� ����� 3 CO
void CO_pt3_RunScript_BlowAndFix()		
{	
	using BlowAndFix; gas = Gas.CO_3; pt = 2;
};
//��������� � �������� �-�� ������ LIN_C0
void CalcAndWriteKoefs_LIN_C0(uint addy)
{	
	using Gas;
	Private.FixPGSAndCalc(addy, Grp.LIN_CO, "PGSco", 3, [N2, CO_2, CO_3] );
};

//����������� ����� 1 CO2
void CO2_pt1_RunScript_BlowAndFix()		
{	
	using BlowAndFix; gas = Gas.N2; pt = 0; grp = Grp.LIN_CO2;
};
//����������� ����� 2 CO2
void CO2_pt2_RunScript_BlowAndFix()		
{	
	using BlowAndFix; gas = Gas.CO2_2; pt = 1;
};
//����������� ����� 3 CO2
void CO2_pt3_RunScript_BlowAndFix()		
{	
	using BlowAndFix; gas = Gas.CO2_3; pt = 2;
};
//����������� ����� 4 CO2
void CO2_pt3_RunScript_BlowAndFix()		
{	
	using BlowAndFix; gas = Gas.CO2_4; pt = 3;
};
//��������� � �������� �-�� ������ LIN_C02
void CalcAndWriteKoefs_LIN_C02(uint addy)
{	
	using Gas;
	Private.FixPGSAndCalc(addy, Grp.LIN_CO2, "PGSco2", 4, [N2, CO2_2, CO2_3, CO2_4] );
};
// ������������� ���� � ����������������
void RunScript_adjust_all() {}




};// namespace adjust_0