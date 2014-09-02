#pragma once
#include "LocalSearch.cpp"
#include <sstream>
#include <iomanip>
#include "Construction.cpp"
#include "AlgoParms.cpp"

class MyITSParms : public AlgoParms
{
private: 
public:
	int PoolSize;
	int SearchSize;
	double MaxPerturbRatio; // MaxPerturb * n
	int MaxPerturbIncreaseRate; // Number of times Perturb cycles around [minPerturb,maxPerturb] to trigger increase
	double ConvergeRatio; // (ConvergeRatio * n)  
	Construction* Construct;
	LocalSearch* Search;
	LocalSearch* Seeder;
	Ratio* Seed; // How many solutions to enhance with Seeder before using Search.
	int SeedCycles; // How many seed cycles of seeder*Seed -> search*Seed before ending with search 
	string* SeedLog;

	MyITSParms() 
	{ 
		PoolSize=5; 
		SearchSize=5; 
		MaxPerturbRatio=1; 
		MaxPerturbIncreaseRate=1;
		ConvergeRatio=.5;
		Search = NULL;
		Seeder = NULL;
		Seed = NULL; 
		Construct = new RandomConstruct();
		SeedCycles = Global::Max;
		SeedLog = NULL;
	}
	inline string Name() { return "MyITS"; }
	inline void Append(stringstream& s)
	{
		s << "LS=" << (Search==NULL ? "." : Search->ToString());
		if (Seeder != NULL)
		{
			s << " seeder=" << Seeder->ToString();
			if (Seed != NULL)
				s << " seed=" << Seed->ToString();
			s << " rounds=" << (SeedCycles==Global::Max ? -1 : SeedCycles);
		} 
		s << " pool=" << PoolSize << " search=" << SearchSize << " perturb=" << MaxPerturbRatio << "n " << "rate=" << MaxPerturbIncreaseRate << " converge=" << ConvergeRatio << "n";
	}

	~MyITSParms() 
	{ 
		delete Search; 
		delete Seeder;
		delete Seed;
		delete Construct;
		delete SeedLog;
	}

	inline string ToString()
	{
		stringstream s;
		return s.str();
	}
};

