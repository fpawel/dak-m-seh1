//---------------------------------------------------------------------------------------------------------------------------------------
//Инициализация коэфициентов
namespace InitializeDevice
{	
string caption = "Инициализация коэфициентов";

//Установить значения к-тов по умолчанию
void PerformInitializeDevice(uint addy)
{		
	using koef;
	for	(int nKoef=0; nKoef<COUNT; ++nKoef)
		if( nKoef!=SER_NUMBER && nKoef!=Ver_PO )
			SetDefault(nKoef, addy);
	Koef.Set( 2010, YEAR, addy);	
	for	(nKoef=0; nKoef<COUNT; ++nKoef)
		Write(nKoef, addy);
	
};


};// namespace adjust_0