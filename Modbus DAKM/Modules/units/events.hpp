//------------------------------------------------------------------------------------
namespace Events
{
//------------------------------------------------------------------------------------
void OnAfterInitializeApplication()
{
	
}
//------------------------------------------------------------------------------------
int OnAfterReadDeviceValue(uint addy, bool isKoefReaded, uint n, double val)
{	
	//Echo( "Прибор %d из %d\n", [ IndexOfAddy(addy), DeviceCount() ] );			
	if(!isKoefReaded)	
	{
		var mConcHlpr = new MetrologyConcentarationHelper(addy, n, val);
		mConcHlpr.ShowConcentration();
	}
}

void OnAfterReadModbusBCD(uint addy, uint nRegister, double val)
{
	//Echo( "%d:%s==%g\n", [addy, FormatModbusRegister( nRegister ), val] );	
}

}; //namespace Events