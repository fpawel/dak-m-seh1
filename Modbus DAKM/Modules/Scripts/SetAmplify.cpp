//---------------------------------------------------------------------------------------------------------------------------------------
void PerformSetAmplifyingCahnal(uint addy, uint nKoef, 
	uint var0, double minVar0, double maxVar0,
	uint var1, double minVar1, double maxVar1,
	uint var2, double minVar2, double maxVar2
	)
{
	TuneAmplifying(addy, var0, nKoef, minVar0, maxVar0, 0.3);	
			
	if( !Var.CheckValueBounds(var0, addy, minVar0, maxVar0) ) return;
	if( !Var.CheckValueBounds(var1, addy, minVar1, maxVar1) ) return;
	if( !Var.CheckValueBounds(var2, addy, minVar2, maxVar2) ) return;
	
}

//Настройка усиления каналов
namespace SetAmplify
{	
string caption = "Настройка усиления каналов";

//Настройка усиления канала CH
void PerformSetAmplifyingCH(uint addy)
{	
	PerformSetAmplifyingCahnal(addy, 40, 
		dch, 3000, 3100, 
		Uchmin, 350, 550, 
		Uchmax, 3450, 3650 );
};
//Настройка усиления канала CO
void PerformSetAmplifyingCO(uint addy)
{	
	 PerformSetAmplifyingCahnal(addy, 41, 
		dco, 3000, 3100, 
		Ucomin, 350, 550, 
		Ucomax, 3450, 3650 );
};
//Настройка усиления канала CO2
void PerformSetAmplifyingCO2(uint addy)
{	
	 PerformSetAmplifyingCahnal(addy, 42, 
		dco2, 3000, 3100, 
		Uco2min, 350, 550, 
		Uco2max, 3450, 3650 );
};
//Настройка Uref
void PerformSetAmplifyingUref(uint addy)
{		 
	 PerformSetAmplifyingCahnal(addy, 43, 
		dref, 3000, 3100, 
		Urefmin, 350, 550, 
		Urefmax, 3450, 3650 );
};
};// namespace adjust_0