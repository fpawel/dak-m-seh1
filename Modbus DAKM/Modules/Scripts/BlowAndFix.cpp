namespace BlowAndFix
{
	uint gas = 0, grp = 0, pt = 0;
	
//������� ��������� ���
void BlowGas()		
{		
	Gas.Blow(gas, BLOW_TIME_SEC);	
};
//��������� ��������� ��������� �����
void SavePoint(uint addy)
{		
	Grp.FixPoint(  addy, grp, pt  ) ;	 	
};
};