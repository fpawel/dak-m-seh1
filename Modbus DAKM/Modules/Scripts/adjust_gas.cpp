//---------------------------------------------------------------------------------------------------------------------------------------
// ���� ����������
namespace adjust_gas
{	
	uint 
		GAS_NUMBER = 0,			// ����� ����������� ���	
		CMD_CODE = 1,			// ����� ������� �� ����������	
		CONC_VAR = 0			// ����� ����������� ����������
	;
//��������� ���������� ���������������� � ��������� �����������
void Perform(uint addy)
{	
	// ������� �������������� ���
	Gas.Blow( GAS_NUMBER, BLOW_TIME_SEC);	
	double 
		ballonConc = Gas.Conc(GAS_NUMBER), 
		lapse = RelativeAccuracyLimitS(CONC_VAR)*ballonConc*0.2; 		
	string 
		gasCaption = Gas.Caption(GAS_NUMBER),
		varCaption = Var.Caption(CONC_VAR);
	var mConcHlpr = new MetrologyConcentarationHelper(addy, -1, -1);
	for( int st=0; st<TRY_ADJUST_COUNT; ++st )
	{
		Echo( "%d:������������� �� ���� \"%s\" %f. ���� %d �� %d.\n", 
			[addy, gasCaption, ballonConc, st+1, TRY_ADJUST_COUNT] );
		Cmd.Send( CMD_CODE, ballonConc, addy );
		Wait( AFTER_SEND_ADJUST_COMAND_DELAY_TIME_SEC );			
		if( mConcHlpr.Check_0_2_Accuracy(CONC_VAR) ) return;		
	}
	string msg = "�� ����������� ����������������.\n" + mConcHlpr.Format_Check_0_2_Accuracy();
	DeviceWarningDialog( addy, msg	);
};


};// namespace adjust_0