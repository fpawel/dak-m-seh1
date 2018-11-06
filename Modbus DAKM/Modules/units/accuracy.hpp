double AbsoluteAccuracyLimit0(uint nVar)
{	
	return Key2Val( nVar, -1,
	[ 
		[Cch,Cco,Cco2], 
		[2.4, 0.012, 0.1] 
	]);	
}
//------------------------------------------------------------------------------------
// Предельное значение относительной погрешности канала измерения.  nVar - номер переменной канала
double RelativeAccuracyLimitS(uint nVar)
{	
	return Key2Val( nVar, -1,
	[ 
		[Cch,Cco,Cco2], 
		[0.05, 0.04, 0.04] 
	]);		
}
//------------------------------------------------------------------------------------
uint GetMetrologyOfVar(uint nVar)
{		
	return Key2Val( nVar, -1,
	[ 
		[Cch,Cco,Cco2], 
		[Metrology.conc_ch, Metrology.conc_co, Metrology.conc_co2]		
	]);		
}
//------------------------------------------------------------------------------------
string FormatAccuracy(double accuracy)
{		
	return accuracy<0 ? "" : FormatFloat("#0.0##", accuracy );
}
//------------------------------------------------------------------------------------
class MetrologyConcentarationHelper
{
	int nConc;
	double delta = -1.0;
	double limD = -1.0;	
	uint gas = Gas.Current();
	uint addy;	
	double val;
	uint n;
	
	function MetrologyConcentarationHelper(uint devAddy, uint nVar, double value)
	{
		InitializeThis(devAddy, nVar, value);
	}	
	
	void InitializeThis(uint devAddy, uint nVar, double value)
	{
		this.n = nVar;		
		this.nConc = GetMetrologyOfVar(n);	
		this.gas = Gas.Current();		
		this.addy = devAddy;
		this.val = value;
		this.delta = -1.0;
		this.limD = -1.0;			
		CalculateAccuracy();
	}
		
	bool NoConc() { return nConc==-1; }
	bool IsAccuracyOK() { return !NoConc() && delta<limD; }
	bool IsAccuracyCalculated() { return !NoConc() && delta>=0 && limD>=0; }	
	bool NoAccuracyError() { return !IsAccuracyCalculated() || IsAccuracyOK(); }	
	bool IsGoodAccuracy() { return IsAccuracyCalculated() && IsAccuracyOK(); }
	
	void ShowConcentration()
	{
		if( NoConc() ) return;		
		Metrology.Set( addy, nConc, FormatFloat("#0.0#",val) );
		Metrology.Set( addy, nConc+1, FormatAccuracy( delta ) );		
		bool mustShowGood = NoAccuracyError();
		Metrology.SetError( addy, nConc, !mustShowGood );
		Metrology.SetError( addy, nConc+1, !mustShowGood );	
	}
	void CalculateAccuracy()
	{	
		if( NoConc() ) return;		
		if (gas==Gas.N2) 
		{				
			limD = AbsoluteAccuracyLimit0(n);
			delta = fabs( val );			
		}
		else
		{		
			if( gas<Gas.Count() &&
				(
					(n==Cch && IsValueIn( gas, [Gas.CH_2, Gas.CH_3] ) ) ||		
					(n==Cco && IsValueIn( gas, [Gas.CO_2, Gas.CO_3] ) ) ||
					(n==Cco2 && IsValueIn( gas,[Gas.CO2_2, Gas.CO2_3, Gas.CO2_4] ) )
				)
			)
			{			
				double ballon = Gas.Conc(gas);
				limD = RelativeAccuracyLimitS(n)*ballon;
				delta = fabs( val - ballon ); 			
			}		
		}		
	}
	bool Check_0_2_Accuracy(uint nVar)
	{		
		this.n = nVar;		
		this.nConc = GetMetrologyOfVar(n);			
		this.val = Var.Read(nVar,addy);
		this.gas = Gas.Current();
		this.delta = -1.0;
		this.limD = -1.0;						
		CalculateAccuracy();		
		//Echo("---Check_0_2_Accuracy--- delta==%g limD==%g\n",[delta, limD]);
		if( !IsAccuracyCalculated() ) 
			throw "Не корректный параметр nVar или текущий газ!";		
		return delta<limD*0.2;
	}
	string Format_Check_0_2_Accuracy()
	{		
		return Format( 
			"Продуваемый газ - \"%s\"\n"+
			"\"%s\"==%g\n"+
			"Абсолютная погрешшность %g\n"+
			"Предельное значение абсолютной погрешности после корректировки %g = 0.2*%g",
		[ Gas.Caption(gas), Var.Caption( n ), val, delta, limD*0.2, limD  ]  );
	}
};