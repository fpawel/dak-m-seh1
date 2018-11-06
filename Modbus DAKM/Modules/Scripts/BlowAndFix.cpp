namespace BlowAndFix
{
	uint gas = 0, grp = 0, pt = 0;
	
//Продуть выбранный газ
void BlowGas()		
{		
	Gas.Blow(gas, BLOW_TIME_SEC);	
};
//Сохранить показания выбранной точки
void SavePoint(uint addy)
{		
	Grp.FixPoint(  addy, grp, pt  ) ;	 	
};
};