
#pragma once
#include <vector>
#include "Algorithm.cpp"
#include "AlgoParms.cpp"
#include "MyITS.cpp"
#include "MyITSParms.cpp"
#include "Grid.cpp"
#include <limits>
#include "Runner.cpp"
#include "LocalSearchAnalysis.cpp"
#include "ParticleSwarmOptimization.cpp"

using namespace std;
enum RunMode { AlgorithmMode, LocalSearchMode, ParameterOptimizationMode};

class Setup
{
private:
	vector<Algorithm*> Algos;
	vector<Instance*> Instances;
	vector<LocalSearch*> Searches;
	vector<Parameter*> Parms;
	string FileName, Description;
	int Runs, RunTime, Iterations, RunsPerInstance, SwarmSize;
	bool Reset;
	RunMode Mode;
public:
	Setup()
	{
		//Mode = ParameterOptimizationMode;
		Mode = AlgorithmMode;
		
		switch (Mode)
		{
			case LocalSearchMode: SetupSearches(); SetupInstances(); break;
			case ParameterOptimizationMode: SetupParameters(); SetupParameterOptimizerInstances(); break;
			default: 
			case AlgorithmMode: SetupAlgos(); SetupInstances(); break;
		}
	}

	void SetupParameterOptimizerInstances()
	{
		// Initialize Benchmarks
		string instanceNames[] = {
	
			//Type I 
			"tai80a",  
			//Type IV
			"tai150b", 
			//Type III
			"tai256c" 
			
			// We can solve within 0-3 minutes.
			// "bur26h","chr25a","els19","esc128","had20","kra32","nug30","rou20","scr20","ste36c","tho40","wil50"
		};
		for (int i=0; i<sizeof(instanceNames)/sizeof(string); ++i) 
			Instances.push_back(new Instance(instanceNames[i]));
	}

	void SetupInstances()
	{
		// Initialize Benchmarks
		string instanceNames[] = {
	
			//Type I 
			"tai80a","tai100a",  //"tai20a","tai25a","tai30a","tai35a","tai40a","tai50a","tai60a",
			//Type IV
			"tai100b", "tai150b", // "lipa90a", //"tai60b","tai80b", ?? Maybe more lipas 
			//Type III
			"tai256c", 
			//Type II 
			"sko90","sko100a","wil100","tho150", // "sko81", "sko100b", "sko100c", "sko100d", "sko100e","sko100f", 
			
			
			// We can solve within 0-3 minutes.
			// "bur26h","chr25a","els19","esc128","had20","kra32","nug30","rou20","scr20","ste36c","tho40","wil50"
		};
		for (int i=0; i<sizeof(instanceNames)/sizeof(string); ++i) 
			Instances.push_back(new Instance(instanceNames[i]));
	}


	void SetupAlgos()
	{
		MyITS* q; MyITSParms* p; 
		RandomWalk* ww; RandomWalkParms* w;
		TabuSearch* tt; TabuSearchParms* t;
		CoreTS* cc; TabuSearchParms* c;
		GLS* gls; BasicGLS* gg; MultiGLS* mg; GLSParms* g;

		// n=80:  35,000n (437n^2) swaps in 15 minutes
		// n=100:  28,000n (280n^2) swaps in 15 minutes

		FileName = "";  
		Runs = 5;
		RunTime = 15 * 60; 
		Iterations = Global::Max;

		
		// - - - - - - - - - - - - - - - SEVENTH - - - - - - - - - - - - - - - - -
		

		//FileName = "sdInterval.4";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 8);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 10);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 12);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 14);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 16);

		//FileName = "sdInterval.5";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, .25);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, .5);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, 1);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, 2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, 4);

		//FileName = "sdInterval.6";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, 6);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, 8);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, 10);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, 12);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, 14);

		//FileName = "evapSinceImp.5";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,3); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,4); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,6); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,8); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,10); g->EvaporateSinceImproveScale=.2;
		//
		//FileName = "evapSinceImp.6";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,3); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,4); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,6); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,8); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,10); g->EvaporateSinceImproveScale=.5;

		//FileName = "evapSinceImpDecay.6";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,.5); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,1); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,2); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,4); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,8); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.9;

		//FileName = "evapSinceImpDecay.7";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,.5); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.7;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,1); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.7;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,2); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.7;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,4); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.7;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,8); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.7;

		//FileName = "evapSinceImpDecay.8";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,.5); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,1); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,2); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,4); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,8); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.5;




		// - - - - - - - - - - - - - - - SIXTH - - - - - - - - - - - - - - - - -
		

		//FileName = "evapSinceImp.1";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,.1); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,.5); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,1); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,2); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,4); g->EvaporateSinceImproveScale=.2;

		//FileName = "evapSinceImp.2";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,8); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,20); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,.5); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,1); g->EvaporateSinceImproveScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,2); g->EvaporateSinceImproveScale=.2;
		//
		//FileName = "evapSinceImp.3";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,.1); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,.5); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,1); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,2); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,4); g->EvaporateSinceImproveScale=.5;

		//FileName = "evapSinceImp.4";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,8); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,20); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,.5); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,1); g->EvaporateSinceImproveScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,0,2); g->EvaporateSinceImproveScale=.5;

		//FileName = "evapSinceImpDecay.1";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,1); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,2); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,4); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,8); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,20); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.9;

		//FileName = "evapSinceImpDecay.2";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,1); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.8;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,2); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.8;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,4); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.8;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,8); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.8;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,20); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.8;
		//
		//FileName = "evapSinceImpDecay.3";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,1); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.7;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,2); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.7;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,4); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.7;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,8); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.7;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,20); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.7;

		//FileName = "evapSinceImpDecay.4";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,1); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.6;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,2); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.6;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,4); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.6;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,8); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.6;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,20); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.6;

		//FileName = "evapSinceImpDecay.5";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,1); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,2); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,4); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,8); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateSinceImproveInterval=new Ratio(0,20); g->EvaporateSinceImproveScale=1; g->EvaporateSinceImproveDecay=.5;

		//FileName = "evapOnInterval.11";  		  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,4); g->EvaporateOnIntervalScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,8); g->EvaporateOnIntervalScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,16); g->EvaporateOnIntervalScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,32); g->EvaporateOnIntervalScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,64); g->EvaporateOnIntervalScale=.2;

		//FileName = "evapOnInterval.12";  		  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,4); g->EvaporateOnIntervalScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,8); g->EvaporateOnIntervalScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,16); g->EvaporateOnIntervalScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,32); g->EvaporateOnIntervalScale=.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,64); g->EvaporateOnIntervalScale=.5;

		//FileName = "evapOnInterval.13";  		  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,8); g->EvaporateOnIntervalScale=.75;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,20); g->EvaporateOnIntervalScale=.75;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,.5); g->EvaporateOnIntervalScale=.75;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,1); g->EvaporateOnIntervalScale=.75;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,2); g->EvaporateOnIntervalScale=.75;

		//FileName = "evapOnInterval.14";  		  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,4); g->EvaporateOnIntervalScale=.75;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,8); g->EvaporateOnIntervalScale=.75;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,16); g->EvaporateOnIntervalScale=.75;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,32); g->EvaporateOnIntervalScale=.75;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,64); g->EvaporateOnIntervalScale=.75;

		//FileName = "multi.1";
		//Algos.push_back(mg= new MultiGLS());g=mg->Parms; g->Threads = 1; 
		//Algos.push_back(mg= new MultiGLS());g=mg->Parms; g->Threads = 2; 
		//Algos.push_back(mg= new MultiGLS());g=mg->Parms; g->Threads = 3; 
		//Algos.push_back(mg= new MultiGLS());g=mg->Parms; g->Threads = 4; 
		//Algos.push_back(mg= new MultiGLS());g=mg->Parms; g->Threads = 5; 

		//FileName = "multi.2";
		//Algos.push_back(mg= new MultiGLS());g=mg->Parms; g->Threads = 6; 
		//Algos.push_back(mg= new MultiGLS());g=mg->Parms; g->Threads = 7; 
		//Algos.push_back(mg= new MultiGLS());g=mg->Parms; g->Threads = 8; 
		//Algos.push_back(mg= new MultiGLS());g=mg->Parms; g->Threads = 9; 
		//Algos.push_back(mg= new MultiGLS());g=mg->Parms; g->Threads = 10;


		// - - - - - - - - - - - - - - - FIFTH - - - - - - - - - - - - - - - - -



		//FileName = "aspGood.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(1); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(2); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(3); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(4); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(5); 
		//
		//FileName = "aspGood.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(6); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(7); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(8); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(9); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(10); 
		
		//FileName = "aspGood.3";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(0,.2); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(0,.3); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(0,.4);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(0,.5); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireGood = new Ratio(0,1); 
		//
		//FileName = "evapOnInterval.5";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,.1); g->EvaporateOnIntervalScale=.10;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,.5); g->EvaporateOnIntervalScale=.10;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,1); g->EvaporateOnIntervalScale=.10;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,2); g->EvaporateOnIntervalScale=.10;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,4); g->EvaporateOnIntervalScale=.10;

		//FileName = "evapOnInterval.6";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,8); g->EvaporateOnIntervalScale=.10;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,20); g->EvaporateOnIntervalScale=.10;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,.5); g->EvaporateOnIntervalScale=.10;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,1); g->EvaporateOnIntervalScale=.10;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,2); g->EvaporateOnIntervalScale=.10;

		//FileName = "evapOnInterval.7";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,.1); g->EvaporateOnIntervalScale=.05;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,.5); g->EvaporateOnIntervalScale=.05;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,1); g->EvaporateOnIntervalScale=.05;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,2); g->EvaporateOnIntervalScale=.05;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,4); g->EvaporateOnIntervalScale=.05;

		//FileName = "evapOnInterval.8";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,8); g->EvaporateOnIntervalScale=.05;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,20); g->EvaporateOnIntervalScale=.05;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,.5); g->EvaporateOnIntervalScale=.05;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,1); g->EvaporateOnIntervalScale=.05;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,2); g->EvaporateOnIntervalScale=.05;

		//FileName = "evapOnInterval.9";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,.1); g->EvaporateOnIntervalScale=.025;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,.5); g->EvaporateOnIntervalScale=.025;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,1); g->EvaporateOnIntervalScale=.025;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,2); g->EvaporateOnIntervalScale=.025;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,4); g->EvaporateOnIntervalScale=.025;

		//FileName = "evapOnInterval.10";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,8); g->EvaporateOnIntervalScale=.025;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,20); g->EvaporateOnIntervalScale=.025;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,.5); g->EvaporateOnIntervalScale=.025;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,1); g->EvaporateOnIntervalScale=.025;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,2); g->EvaporateOnIntervalScale=.025;


		// - - - - - - - - - - - - - - - FOURTH - - - - - - - - - - - - - - - - -


		//FileName = "steep.ts.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,.5); g->Steep.TabuSearchSwaps=new Ratio(0,2); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,1); g->Steep.TabuSearchSwaps=new Ratio(0,2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,2); g->Steep.TabuSearchSwaps=new Ratio(0,2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,4); g->Steep.TabuSearchSwaps=new Ratio(0,2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,6); g->Steep.TabuSearchSwaps=new Ratio(0,2);

		//FileName = "steep.ts.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,10); g->Steep.TabuSearchSwaps=new Ratio(0,2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,16); g->Steep.TabuSearchSwaps=new Ratio(0,2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,0,.5); g->Steep.TabuSearchSwaps=new Ratio(0,2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,0,1); g->Steep.TabuSearchSwaps=new Ratio(0,2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,0,2); g->Steep.TabuSearchSwaps=new Ratio(0,2);

		//FileName = "steep.ts.3";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,.5); g->Steep.TabuSearchSwaps=new Ratio(0,10); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,1); g->Steep.TabuSearchSwaps=new Ratio(0,10);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,2); g->Steep.TabuSearchSwaps=new Ratio(0,10);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,4); g->Steep.TabuSearchSwaps=new Ratio(0,10);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,6); g->Steep.TabuSearchSwaps=new Ratio(0,10);

		//FileName = "steep.ts.4";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,10); g->Steep.TabuSearchSwaps=new Ratio(0,10);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,16); g->Steep.TabuSearchSwaps=new Ratio(0,10);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,0,.5); g->Steep.TabuSearchSwaps=new Ratio(0,10);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,0,1); g->Steep.TabuSearchSwaps=new Ratio(0,10);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.TabuLength=new Ratio(0,0,2); g->Steep.TabuSearchSwaps=new Ratio(0,10);


		//FileName = "steep.dist.ts.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,.5); g->Steep.TabuSearchSwaps=new Ratio(0,2); g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,1); g->Steep.TabuSearchSwaps=new Ratio(0,2);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,2); g->Steep.TabuSearchSwaps=new Ratio(0,2);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,4); g->Steep.TabuSearchSwaps=new Ratio(0,2);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,6); g->Steep.TabuSearchSwaps=new Ratio(0,2);g->Steep.IsMutateByDistance=true;

		//FileName = "steep.dist.ts.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,10); g->Steep.TabuSearchSwaps=new Ratio(0,2);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,16); g->Steep.TabuSearchSwaps=new Ratio(0,2);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,0,.5); g->Steep.TabuSearchSwaps=new Ratio(0,2);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,0,1); g->Steep.TabuSearchSwaps=new Ratio(0,2);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,0,2); g->Steep.TabuSearchSwaps=new Ratio(0,2);g->Steep.IsMutateByDistance=true;

		//FileName = "steep.dist.ts.3";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,.5); g->Steep.TabuSearchSwaps=new Ratio(0,10); g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,1); g->Steep.TabuSearchSwaps=new Ratio(0,10);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,2); g->Steep.TabuSearchSwaps=new Ratio(0,10);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,4); g->Steep.TabuSearchSwaps=new Ratio(0,10);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,6); g->Steep.TabuSearchSwaps=new Ratio(0,10);g->Steep.IsMutateByDistance=true;

		//FileName = "steep.dist.ts.4";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,10); g->Steep.TabuSearchSwaps=new Ratio(0,10);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,16); g->Steep.TabuSearchSwaps=new Ratio(0,10);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,0,.5); g->Steep.TabuSearchSwaps=new Ratio(0,10);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,0,1); g->Steep.TabuSearchSwaps=new Ratio(0,10);g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.TabuLength=new Ratio(0,0,2); g->Steep.TabuSearchSwaps=new Ratio(0,10);g->Steep.IsMutateByDistance=true;

		//FileName = "steep.dist.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0); g->Steep.PerturbMax=new Ratio(0,1); g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9); g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.8); g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.7); g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.IsMutateByDistance=true;

		//FileName = "steep.dist.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.2); g->Steep.PerturbMax=new Ratio(0,.8); g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.2); g->Steep.PerturbMax=new Ratio(0,.6); g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.3); g->Steep.PerturbMax=new Ratio(0,.8); g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.3); g->Steep.PerturbMax=new Ratio(0,.6); g->Steep.IsMutateByDistance=true;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.3); g->Steep.PerturbMax=new Ratio(0,.5); g->Steep.IsMutateByDistance=true;

		//FileName = "steep.3";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.0); g->Steep.PerturbMax=new Ratio(0,.1); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.0); g->Steep.PerturbMax=new Ratio(0,.2); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.0); g->Steep.PerturbMax=new Ratio(0,.3); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.0); g->Steep.PerturbMax=new Ratio(0,.4); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.0); g->Steep.PerturbMax=new Ratio(0,.5); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.0); g->Steep.PerturbMax=new Ratio(0,.6); 

		//FileName = "steep.4";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.0); g->Steep.PerturbMax=new Ratio(0,.7); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.0); g->Steep.PerturbMax=new Ratio(0,.8); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.0); g->Steep.PerturbMax=new Ratio(0,.9); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.2); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.3); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.4); 
		//
		//FileName = "steep.5";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,1); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.2); g->Steep.PerturbMax=new Ratio(0,.3); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.2); g->Steep.PerturbMax=new Ratio(0,.4); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.2); g->Steep.PerturbMax=new Ratio(0,.5); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.2); g->Steep.PerturbMax=new Ratio(0,.7); 
		//
		//FileName = "steep.6";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.2); g->Steep.PerturbMax=new Ratio(0,.9); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.2); g->Steep.PerturbMax=new Ratio(0,1); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.3); g->Steep.PerturbMax=new Ratio(0,.4); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.3); g->Steep.PerturbMax=new Ratio(0,.7); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.3); g->Steep.PerturbMax=new Ratio(0,.9); 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.3); g->Steep.PerturbMax=new Ratio(0,1); 

		//FileName = "evapOnInterval.1";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,.1); g->EvaporateOnIntervalScale=.20;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,.5); g->EvaporateOnIntervalScale=.20;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,1); g->EvaporateOnIntervalScale=.20;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,2); g->EvaporateOnIntervalScale=.20;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,4); g->EvaporateOnIntervalScale=.20;

		//FileName = "evapOnInterval.2";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,8); g->EvaporateOnIntervalScale=.20;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,20); g->EvaporateOnIntervalScale=.20;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,.5); g->EvaporateOnIntervalScale=.20;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,1); g->EvaporateOnIntervalScale=.20;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,2); g->EvaporateOnIntervalScale=.20;

		//FileName = "evapOnInterval.3";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,.1); g->EvaporateOnIntervalScale=.50;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,.5); g->EvaporateOnIntervalScale=.50;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,1); g->EvaporateOnIntervalScale=.50;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,2); g->EvaporateOnIntervalScale=.50;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,4); g->EvaporateOnIntervalScale=.50;

		//FileName = "evapOnInterval.4";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,8); g->EvaporateOnIntervalScale=.50;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,20); g->EvaporateOnIntervalScale=.50;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,.5); g->EvaporateOnIntervalScale=.50;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,1); g->EvaporateOnIntervalScale=.50;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateInterval=new Ratio(0,0,2); g->EvaporateOnIntervalScale=.50;

				
		// - - - - - - - - - - - - - - - THIRD - - - - - - - - - - - - - - - - -

		
		//FileName = "lambdaPow.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=1.0; g->LambdaPower=1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=1.0; g->LambdaPower=1.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=1.0; g->LambdaPower=2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=1.0; g->LambdaPower=2.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=1.0; g->LambdaPower=3;
		//
		//FileName = "lambdaPow.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=2.0; g->LambdaPower=1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=2.0; g->LambdaPower=1.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=2.0; g->LambdaPower=2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=2.0; g->LambdaPower=2.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=2.0; g->LambdaPower=3;

		//FileName = "lambdaPow.3";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=4.0; g->LambdaPower=1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=4.0; g->LambdaPower=1.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=4.0; g->LambdaPower=2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=4.0; g->LambdaPower=2.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=4.0; g->LambdaPower=3;
		//
		//FileName = "lambdaPow.4";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=8.0; g->LambdaPower=1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=8.0; g->LambdaPower=1.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=8.0; g->LambdaPower=2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=8.0; g->LambdaPower=2.5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=8.0; g->LambdaPower=3;

		//FileName = "bestMovePr.1";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMovePr=.05;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMovePr=.1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMovePr=.15;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMovePr=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMovePr=.25;

		//FileName = "bestMovePr.2";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMovePr=.3;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMovePr=.35;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMovePr=.4;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMovePr=.45;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMovePr=.5;	

		//FileName = "evapOnSwap.1";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnSwapScale=.0005;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnSwapScale=.001;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnSwapScale=.005;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnSwapScale=.01;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnSwapScale=.05;

		//FileName = "evapOnSwap.2";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnSwapScale=.1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnSwapScale=.15;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnSwapScale=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnSwapScale=.25;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnSwapScale=.3;






		// - - - - - - - - - - - - - - - SECOND - - - - - - - - - - - - - - - - -

		//FileName = "aspLate.randMove.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->RandomMovePr=.1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->RandomMovePr=.25;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->RandomMovePr=.35;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,4); g->RandomMovePr=.1;

		//FileName = "aspLate.randMove.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,4); g->RandomMovePr=.25;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,4); g->RandomMovePr=.35;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,6); g->RandomMovePr=.1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,6); g->RandomMovePr=.25;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,6); g->RandomMovePr=.35;

		//FileName = "aspLate.lambda.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->Lambda = .2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->Lambda = .4;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->Lambda = .6;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->Lambda = .8;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->Lambda = 1;

		//FileName = "aspLate.lambda.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->Lambda = 1.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->Lambda = 1.4;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->Lambda = 1.6;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->Lambda = 1.8;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2); g->Lambda = 2.0;

		//FileName = "aspLate.min.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,1.25);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,1.5);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,1.75);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2.25);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2.5);

		//FileName = "aspLate.min.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2.75);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,3);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,3.25);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,3.5);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,3.75);

		//FileName = "randMove.min.1";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.21;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.22;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.23;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.24;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.25;

		//FileName = "randMove.min.2";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.26;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.27;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.28;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.29;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.3;

		//FileName = "evapOnImp.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnImprovePr=0;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnImprovePr=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnImprovePr=.4;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnImprovePr=.6;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnImprovePr=.8;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->EvaporateOnImprovePr=1;



		// - - - - - - - -   FIRST - - - - - - - - - - - - - -

		//FileName = "base";  
		//Algos.push_back(tt=new TabuSearch());t=tt->Parms; t->U = new Ratio(0,2); t->T = new Ratio(0,0,5);
		//Algos.push_back(gg= new BasicGLS());g=gg->Parms; g->IsAspireBest=false;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->IsAspireBest=false;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Lambda=.6; g->RandomMovePr = .2; g->IsAspireBest=true;
		
		//FileName = "base.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; 
		//Algos.push_back(ww=new RandomWalk());w=ww->Parms; w->Search= new BestTwoExchange();
		//Algos.push_back(ww=new RandomWalk());w=ww->Parms; w->Search= new FastDescent(); 
		//Algos.push_back(ww=new RandomWalk());w=ww->Parms; w->Search= new Lin(1,new Ratio(0,1)); 

		//FileName = "other";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->IsSteepestDescentAlways=true; 

		//FileName = "randMove.1";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.05;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.15;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.25;

		//FileName = "randMove.2";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.3;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.35;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.4;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.45;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->RandomMovePr=.5;		

		//FileName = "bestMove.1";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMoveInterval=new Ratio(0,.1);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMoveInterval=new Ratio(0,.5);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMoveInterval=new Ratio(0,1);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMoveInterval=new Ratio(0,1.5);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMoveInterval=new Ratio(0,2);

		//FileName = "bestMove.2";  
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMoveInterval=new Ratio(0,4);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMoveInterval=new Ratio(0,6);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMoveInterval=new Ratio(0,10);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMoveInterval=new Ratio(0,0,.5);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->BestMoveInterval=new Ratio(0,0,1);

		//FileName = "aspLate.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,.1);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,.5);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,1);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,4);

		//FileName = "aspLate.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,6);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,8);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,10);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,20);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->AspireLate=new Ratio(0,0,999);

		//FileName = "noise.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->PenaltyNoisePr = 0;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->PenaltyNoisePr = .01;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->PenaltyNoisePr = .02;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->PenaltyNoisePr = .04;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->PenaltyNoisePr = .06;

		//FileName = "noise.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->PenaltyNoisePr = .08;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->PenaltyNoisePr = .1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->PenaltyNoisePr = .15;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->PenaltyNoisePr = .2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->PenaltyNoisePr = .25;

		//FileName = "sdInterval.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, .1);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, .2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, .3);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, .5);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, .7);

		//FileName = "sdInterval.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, .9);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 1);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 1.5);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 4);

		//FileName = "sdInterval.3";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, .1);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, .25);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, .5);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, 1);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, 2);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->SteepestDescentInterval= new Ratio(0, 0, 4);


		//FileName = "penaltyUtil";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByUtility; g->penaltyAmount = .1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByUtility; g->penaltyAmount = .5;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByUtility; g->penaltyAmount = 1;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByUtility; g->penaltyAmount = 2;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByUtility; g->penaltyAmount = 3;

		//FileName = "penalty";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByUtility; 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByUtility; 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByCost; 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByCost; 

		//FileName = "penalty.evap.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByUtility; g->EvaporateMode=EvapNone; g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByUtility; g->EvaporateMode=EvapByPenalty; g->EvaporateAmount=.9; 
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByUtility; g->EvaporateMode=EvapAllByPenalty; g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByUtility; g->EvaporateMode=EvapByLate; g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByUtility; g->EvaporateMode=EvapAllByLate; g->EvaporateAmount=.9;

		//FileName = "penalty.evap.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByUtility; g->EvaporateMode=EvapNone;  g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByUtility; g->EvaporateMode=EvapByPenalty;  g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByUtility; g->EvaporateMode=EvapAllByPenalty;  g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByUtility; g->EvaporateMode=EvapByLate;  g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByUtility; g->EvaporateMode=EvapAllByLate;  g->EvaporateAmount=.9;


		//FileName = "penalty.evap.3";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByCost; g->EvaporateMode=EvapNone;   g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByCost; g->EvaporateMode=EvapByPenalty;   g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByCost; g->EvaporateMode=EvapAllByPenalty;   g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByCost; g->EvaporateMode=EvapByLate;   g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseByCost; g->EvaporateMode=EvapAllByLate;   g->EvaporateAmount=.9;

		//FileName = "penalty.evap.4";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByCost; g->EvaporateMode=EvapNone;   g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByCost; g->EvaporateMode=EvapByPenalty;   g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByCost; g->EvaporateMode=EvapAllByPenalty;   g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByCost; g->EvaporateMode=EvapByLate;   g->EvaporateAmount=.9;
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->penaltyMode=IncreaseAllByCost; g->EvaporateMode=EvapAllByLate;   g->EvaporateAmount=.9;

		//FileName = "steep.1";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0); g->Steep.PerturbMax=new Ratio(0,1);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.9);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.8);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.7);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.1); g->Steep.PerturbMax=new Ratio(0,.5);

		//FileName = "steep.2";
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.2); g->Steep.PerturbMax=new Ratio(0,.8);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.2); g->Steep.PerturbMax=new Ratio(0,.6);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.3); g->Steep.PerturbMax=new Ratio(0,.8);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.3); g->Steep.PerturbMax=new Ratio(0,.6);
		//Algos.push_back(gls= new GLS());g=gls->Parms; g->Steep.PerturbMin=new Ratio(0,.3); g->Steep.PerturbMax=new Ratio(0,.5);
	}


	void SetupParameters()
	{
		MyITS* q; MyITSParms* p; 
		RandomWalk* ww; RandomWalkParms* w;
		TabuSearch* tt; TabuSearchParms* t;
		CoreTS* cc; TabuSearchParms* c;
		GLS* gls; BasicGLS* gg; MultiGLS* mg; GLSParms* g;


		SwarmSize = 10;
		
		Runs = 5;
		RunTime = 10 * 60; // 10 minutes
		
		Runs = 1;
		RunTime = 1 * 10; // 10 seconds 
		Iterations = Global::Max;

		FileName = "";
		//FileName = "pso.evapOnInt";  
		
		Algos.push_back(gls= new GLS());g=gls->Parms;
		
		Parms.push_back(new Parameter("Lambda", new Ratio(.1), new Ratio(10), false)); 
		Parms.push_back(new Parameter("EvaporateInterval", new Ratio(0,.1), new Ratio(0,0,64), true)); 
		Parms.push_back(new Parameter("EvaporateOnIntervalScale", new Ratio(.025), new Ratio(1.0), false));
	}

	void SetupSearches() 
	{
		FileName = "";
		RunsPerInstance = 2;
		Runs = 2;
		Reset = false;

		Searches.push_back(new RoTS(new Ratio(0,0,1), false, new Ratio(0,2), new Ratio(0,0,99)));
		Searches.push_back(new RoTS(new Ratio(0,0,1), true, new Ratio(0,1), new Ratio(0,0,99)));
		Searches.push_back(new Lin(1, new Ratio(0,1)));
		Searches.push_back(new Lin(1, new Ratio(0,.9)));
		//Searches.push_back(new Concat(new Lin(1,new Ratio(0,.9)), new RoTS(new Ratio(0,0,1), true, new Ratio(0,1), new Ratio(0,0,99))));
		//Searches.push_back(new Concat(new RoTS(new Ratio(0,10), true, new Ratio(0,1), new Ratio(0,0,99)), new Lin(1,new Ratio(0,.9))));
	}



	void Run()
	{

		if (Mode == LocalSearchMode)
		{
			LocalSearchAnalysis analyze(Searches);
			analyze.Analyze(Instances, RunsPerInstance, Runs, FileName == "" ? "" : Reset ? FileName + ".reset" : FileName, Reset); 
		}
		else if (Mode == AlgorithmMode)
		{
			Grid grid(FileName, Description, Runs, RunTime, Iterations, Algos);
			grid.PrintHeader();
			for (int i=0; i<Instances.size(); ++i)
				for (int j=0; j<Algos.size(); ++j)
				{
					Runner run(*Instances[i], Runs, RunTime, Iterations); 
					Result* result = Algos[j]->Run(run);
					grid.Print(*result);
					delete result;
				}
			grid.PrintFooter();
		}
		else if (Mode == ParameterOptimizationMode)
		{
			ParticleSwarmOptimization particle(FileName, Algos[0], Parms, Instances);
			particle.Run(SwarmSize, Iterations, Runs, RunTime);
		}
	}


	~Setup()
	{
		for (int i=0; i<Instances.size(); ++i)
			delete Instances[i];
		for (int i=0; i<Algos.size(); ++i)
			delete Algos[i];
		for (int i=0; i<Searches.size(); ++i)
			delete Searches[i];
		for (int i=0; i<Parms.size(); ++i)
			delete Parms[i];
	}
};