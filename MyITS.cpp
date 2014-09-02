#pragma once
#include <vector>
#include "Instance.cpp"
#include "Solution.cpp"
#include "MyITSParms.cpp"
#include "Pool.cpp"
#include <float.h>
#include <fstream>
#include <iomanip>
#include <time.h>
#include "Result.cpp"
#include "Runner.cpp"
#include "Algorithm.cpp"

class MyITS : public Algorithm
{
private: 
	Pool *Best, *Search;
	int SearchReplaceCount, BestReplaceCount;
public:
	MyITSParms Parms;
	MyITS() : Best(NULL),Search(NULL), SearchReplaceCount(0), BestReplaceCount(0) {}
	
	inline AlgoParms *GetParms() { return &Parms; }

	inline void PreRun(Runner& runner)
	{
		SearchReplaceCount = BestReplaceCount = 0;
		Parms.Construct->Initialize(Problem->Size);
		Best = new Pool(runner, Parms);  
		Search = new Pool(runner, Parms);
		Best->Generate(Parms.PoolSize);
		Search->Generate(Parms.SearchSize);
		SearchReplaceCount += Best->RepeatMerge(*Search);
	}

	inline double Iterate(Runner& runner)
	{
		Pool *p = Best->PerturbPool();
		BestReplaceCount += Best->Merge(*p);
		for (int i=0; i<Best->Size(); ++i)
			if (p->Solutions[i] == NULL)
				Best->ReplacedSolution(i);
		delete p;

		Pool *q = Search->PerturbPool();
		SearchReplaceCount += Best->Merge(*q);
		for (int i=0; i<q->Size(); ++i)
		{
			if (q->Solutions[i] == NULL || Search->IsConvergeAt(i)) // replaced a solution in Best pool.  Need to create new random solution in its old spot.
			{	
				if (q->Solutions[i] == NULL) 
					Search->ReplacedSolution(i); 
				Search->GenerateAt(i);
				SearchReplaceCount += Best->RepeatMerge(*Search, i);
			}
			else // didn't replace any solution in Best pool. See if it is better than original solution from which it was derived.
				Search->TryMergeAt(i, q->Solutions[i]);
		}
		delete q;
		return runner.GetFitness();
	}

	void PostRun()
	{
		delete Best;
		delete Search;
	}

	inline void PrintDetail(Runner &runner)
	{

		Algorithm::PrintDetail(runner,false);
		for (int i=0; i<Best->Size(); ++i)
			cout << " " << Best->PerturbMax[i];
		cout << " (" << BestReplaceCount << ")";
		for (int i=0; i<Search->Size(); ++i)
			cout << " " << Search->PerturbMax[i];
		cout << " (" << SearchReplaceCount << "," << Search->GenerationCount << ")" << endl;
	}


};
