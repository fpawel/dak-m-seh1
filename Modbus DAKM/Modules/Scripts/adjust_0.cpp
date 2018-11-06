//---------------------------------------------------------------------------------------------------------------------------------------
//Цикл калибровки нуля
namespace adjust_0
{	
string caption = "Корректировка нуля";
// Выполнить калибровку нуля шкалы с заданными параметрами
void Perform(uint addy)
{	
	Gas.Blow( Gas.N2, BLOW_TIME_SEC);	
	var mConcHlpr = new MetrologyConcentarationHelper(addy, -1, -1);
	for( int st=0; st<TRY_ADJUST_COUNT; ++st )
	{
		Echo( "%d:корректировка нуля. Цикл %d из %d.\n", [addy, st+1, TRY_ADJUST_COUNT] );
		Cmd.Send( Cmd.adjust_0, 0, addy );	
		Wait(AFTER_SEND_ADJUST_COMAND_DELAY_TIME_SEC);	
		if( mConcHlpr.Check_0_2_Accuracy(Cch) && 
			mConcHlpr.Check_0_2_Accuracy(Cco) &&
			mConcHlpr.Check_0_2_Accuracy(Cco2))
			return;		
	}	
	string msg = "Не калибруется \"ноль\".\n" + mConcHlpr.Format_Check_0_2_Accuracy();
	DeviceWarningDialog( addy, msg	);
};

};// namespace adjust_0