variant X,Y,K,pgs;

namespace LIN_CH
{
	variant Cout, PGS;
	namespace Info
	{
		string name = "Линеаризация ф-ии преобразов. к. CH";
		int koef = 16;		
	};			
};


namespace SCALE_CH_0_T
{
	variant Var2, Tpp;
	namespace Info 
	{
		string name = "Комп. вл. темп. на нулев. показ. к. СН";	
		int koef=23;
	};
	void Calculate()
	{		
		X[0] = Tpp[0];		X[1] = Tpp[1];		X[2] = Tpp[2];
		Y[0] = -Var2[0];
		Y[1]= -Var2[1];
		Y[2]= -Var2[2];
		
	}
};


namespace SCALE_CH_S_T
{
	variant Var0, Var2, Tpp;
	namespace Info 
	{
		string name = "Комп. влиян. темп-ры на чувст. по к. CH";
		int koef=26;
	};
	void Calculate()
	{
		variant vk = [	
			Var2[0]-Var0[0], 
			Var2[1]-Var0[1],
			Var2[2]-Var0[2]
		];
		X[0] = Tpp[0];		X[1] = Tpp[1];		X[2] = Tpp[2];
		Y[0]=vk[1]/vk[0];
		Y[1]=1;
		Y[2]=vk[1]/vk[2];
	}
};


