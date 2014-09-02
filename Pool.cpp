#pragma once
#include "Instance.cpp"
#include "Solution.cpp"
#include "MyITSParms.cpp"
#include "Global.cpp"
#include <limits.h>
#include <vector>
#include "Runner.cpp"

class Pool
{
private:
	MyITSParms& Parms;
	Instance& Problem;
	int PerturbUB;
	int ConvergeAt;
	int Seed;
	Runner& Run;
	fstream SeedLog;
	int SeedSize;
public:
	vector<int> Perturb, PerturbMax, Cycles, SearchCount, LastSearch;
	vector<Solution*> Solutions, SeedSolutions;
	int GenerationCount;
	Solution* BestSolution;
	Pool(Runner& runner, MyITSParms& parms) : Run(runner), Problem(runner.Problem), Parms(parms), GenerationCount(0), BestSolution(NULL)
	{
		if (Parms.SeedLog != NULL) Global::OpenFile(*Parms.SeedLog, SeedLog, true);
		Seed = Parms.Seed == NULL ? 0 : Parms.Seed->Calculate(Problem.Size); 
		PerturbUB = Global::Constrain((int)(Parms.MaxPerturbRatio * Problem.Size+.5), 2, Problem.Size);
		ConvergeAt = Global::Constrain((int)(Parms.ConvergeRatio * Problem.Size), 2, PerturbUB); 
	}

	~Pool()
	{
		for (int i=0;i<(int)Solutions.size(); ++i)
			delete Solutions[i];
	}

	inline void Generate(int size)
	{
		SeedSize = size;
		Clear();
		for (int i=0; i<size; ++i)
			Add();
	}


	inline void GenerateAt(int index)
	{
		Solution* s = Parms.Construct->Generate(Problem);
		//for (int i=0; i<s->Size(); ++i)
		//	cout << (*s)[i] << " ";
		//cout << endl;
		delete Solutions[index];
		Solutions[index] = s;
		Enhance(*s,index);
		ResetPerturbAt(index);
		++GenerationCount;
		Update(*s);
	}

	inline Solution*& operator[](int i) { return Solutions[i]; }
    inline int Size() { return Solutions.size(); }
	inline void Clear() { Solutions.clear(); Perturb.clear(); PerturbMax.clear(); Cycles.clear(); }
	inline void Add(Solution* s=NULL)
	{
		Solutions.push_back(s); Perturb.push_back(0); PerturbMax.push_back(0); Cycles.push_back(0); SearchCount.push_back(0); SeedSolutions.push_back(NULL); LastSearch.push_back(0);
		ResetPerturbAt(Solutions.size()-1);
		if (s == NULL)
			GenerateAt(Solutions.size()-1);
		else
			Update(*s);
	}

	Pool* PerturbPool()
	{
		int j=0;
		double cost;
		Pool *pool = new Pool(Run, Parms);
		Solution rand(Problem);
		int mod = Problem.Size % 2 == 0 ? Problem.Size : Problem.Size-1;
		for (int i=0; i<(int)Solutions.size(); ++i)
		{
			Solution *s = new Solution(*Solutions[i]);
			for (int swaps=0; swaps<Perturb[i]; j = (j+2)%mod, ++swaps)
			{
				if (j==0) rand.Randomize();
				cost = s->SwapCost(rand[j],rand[j+1]);
				s->Swap(rand[j],rand[j+1],&cost);
			}
			IncreasePerturbAt(i);
			Enhance(*s,i);
			pool->Add(s);
		}
		return pool;
	}

	inline void ResetPerturbAt(int index)
	{
		PerturbMax[index] = 2;
		Perturb[index] = 2;
		Cycles[index] = 0;
	}

	inline void IncreasePerturbAt(int i)
	{
		++Perturb[i];
		if (Perturb[i] > PerturbMax[i])
		{
			++Cycles[i];
			if (Cycles[i] % Parms.MaxPerturbIncreaseRate == 0)
				++PerturbMax[i];
			PerturbMax[i] = Global::Constrain(PerturbMax[i], 2, PerturbUB);
			Perturb[i] = 2;
		}
	}

	// Call this if a solution was the source of a best solution (replaced a solution in the pool somewhere else)
	inline void ReplacedSolution(int i)  
	{
		// Perturb[i] must be a good place to be perturbing, so set max back down to it.
		Perturb[i] = max(2, Perturb[i]-1);
		PerturbMax[i] = Perturb[i];  
	}


	// Keep merging solutions and generating new solutions in merging pool until the all solutions are worse than all solutions in this pool.
	inline int RepeatMerge(Pool& pool, int index=-1)
	{
		int mergeCount = 0;
		bool merged;

		for (int i=0; i<pool.Size(); ++i)
		{
			if (index != -1)
				i = index;
			do
			{
				if (merged = MergeSingle(pool.Solutions[i]))
				{
					++mergeCount;
					pool.ReplacedSolution(i);
					pool.GenerateAt(i);
				}
			} while (merged);

			if (index != -1)
				break;
		}
		return mergeCount;
	}

	inline int Merge(Pool& pool, bool nullOnMerge = true)
	{
		int mergeCount = 0;
		for (int i=0; i<pool.Size(); ++i)
			if (MergeSingle(pool.Solutions[i], nullOnMerge))
				mergeCount++;
		return mergeCount;
	}

	inline bool MergeSingle(Solution*& solution, bool nullOnMerge = true)
	{
		bool isBetter = false;
		for (int i=0; i<Size(); ++i)
			if (solution->GetFitness() < Solutions[i]->GetFitness())
			{	
				isBetter = true;
				break;
			}
		if (!isBetter) 
			return false;

		int closestIndex = 0, closestDistance = Global::Max, currentDistance;
		for (int i=0; i<Size(); ++i)
		{
			currentDistance = solution->Distance(*Solutions[i]);
			if (currentDistance < closestDistance)
			{
				closestIndex = i;
				closestDistance = currentDistance;
			}
		}
		return TryMergeAt(closestIndex, solution, nullOnMerge);
	}


	inline bool TryMergeAt(int index, Solution*& solution, bool nullOnMerge = true)
	{

		if (solution->GetFitness() < Solutions[index]->GetFitness())
		{
			delete Solutions[index];
			ResetPerturbAt(index);
			if (nullOnMerge)
			{
				Solutions[index] = solution;
				solution = NULL;
			}
			else 
				Solutions[index] = new Solution(*solution);
			Update(*Solutions[index]);
			return true;
		}
		return false;
	}

	inline bool IsConvergeAt(int index)
	{
		return Perturb[index] == ConvergeAt;
	}

	inline void Enhance(Solution& solution, int poolIndex)
	{
		bool noSeed = Seed == 0 || (SearchCount[poolIndex] / Seed) % 2 == 1 || SearchCount[poolIndex] / (2*Seed) >= Parms.SeedCycles;
		if (Parms.Search != NULL && (Parms.Seeder == NULL || noSeed))
		{
			LogSearchResults(poolIndex); 
			Parms.Search->Enhance(solution, Run.Iteration);
			LastSearch[poolIndex] = 1;
		}
		else if (Parms.Seeder != NULL)
		{
			LogSearchResults(poolIndex);
			Parms.Seeder->Enhance(solution, Run.Iteration);
			LastSearch[poolIndex] = 2;
		}
		++SearchCount[poolIndex];
		Parms.Construct->UpdateFrom(solution);
	}

	inline void LogSearchResults(int i)
	{
		if (!SeedLog.is_open() || SearchCount[i] % Seed > 0) // Only output once every seed interval.  
			return;

		Solution *s1 = SeedSolutions[i];
		Solution *s2 = Solutions[i];
		if (i == 0)
			SeedLog << Problem.InstanceName << ", " << SearchCount[i] << ", ";
		SeedLog << LastSearch[i] << ", " << s2->GetDeviation() << ", "; 
		if (s1 != NULL)
			SeedLog  << s2->HammingDistance(*s1) << ", " << s2->SignDistance(*s1) << ", ";
		else
			SeedLog << "0,0,";

		if (i == SeedSize-1) // Dont use solutions.size() because it might be still populating the pool from inital generate function.
			SeedLog << endl;

		SeedLog.flush();

		delete SeedSolutions[i];
		SeedSolutions[i] = new Solution(*s2);
	}

	inline void Update(Solution& solution)
	{
		Run.Update(solution.GetFitness());
	}
};
