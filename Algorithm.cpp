#pragma once
#include <vector>
#include "Instance.cpp"
#include "Solution.cpp"
#include <float.h>
#include <fstream>
#include <iomanip>
#include <time.h>
#include "Result.cpp"
#include "Runner.cpp"
#include "AlgoParms.cpp"
#include <fstream>
#include <sstream>
#include <queue>



class Algorithm
{
private: 
	double Fitness;
	fstream *Output;
public:
	Instance *Problem;

	Algorithm() : Problem(NULL), Output(NULL) 
	{
		
	} 
	~Algorithm() 
	{ 
		if (Output!=NULL) Output->close(); 
		delete Output; 
	}

	Result* Run(Runner& runner)
	{
		Problem = &runner.Problem;
		Result* result = new Result(*Problem, GetParms()->Key);
		double fitness;

		for (int i=0; i<runner.Runs; ++i)
		{
			runner.Run();
			
			if (GetParms()->Debug)
			{
				stringstream ss;
				ss << endl << Problem->ToString() << " run=" << i << endl ;
				Debug(ss.str());
			}

			PreRun(runner);
			while (!runner.IsDone()) 
			{
				fitness = Iterate(runner);
				runner.Update(fitness);
				Print(runner);
				runner.Iterate();
			}
			PostRun();
			Print(runner);
			result->Add(runner);
		}
		return result;
	}

	inline void Print(Runner &runner)
	{
		if (runner.Runs != 1 || Fitness == runner.GetFitness()) return;
		Fitness = runner.GetFitness();
		PrintDetail(runner);
	}

	inline void Debug(string debug)
	{
		if (!GetParms()->Debug)
			return;
		if (Output == NULL)
		{
			Output = new fstream();
			Global::OpenFile("", *Output, true);
			*Output << GetParms()->ToString() << endl;
		}
		*Output << debug;
		Output->flush();
	}

	virtual inline void CreateParms(bool deleteOld = true) = 0;
	virtual inline AlgoParms *GetParms() = 0;
	virtual inline void PreRun(Runner& runner) {};
	virtual inline double Iterate(Runner& runner) = 0;
	virtual inline void PostRun() {};
	virtual inline void PrintDetail(Runner& runner, bool newline=true) 
	{
		cout << setw(2) << GetParms()->Key <<  ":" << setw(4) << runner.RunTime() << "s " << setw(10) << int(runner.GetFitness()) << " " << setw(5) << runner.Deviation() << "%";
		if (newline)
			cout << endl;
	}
};


class RandomWalk : public Algorithm
{
public:
	RandomWalkParms *Parms;

	RandomWalk() { CreateParms(false); }
	~RandomWalk() { delete Parms; }
	inline void CreateParms(bool deleteOld = true) { if (deleteOld) delete Parms; Parms = new RandomWalkParms(); }
	inline AlgoParms *GetParms() { return Parms; }
	
	inline double Iterate(Runner& runner)
	{
		Solution s(*Problem);
		Parms->Search->Enhance(s, runner.Runs, Global::Max, &runner);
		return s.GetFitness();
	}

};

class BasicGLS : public Algorithm
{
	public:
	GLSParms *Parms;
	Solution *Best, *Current;
	int n;
	double **Penalty, **Delta;
	double lambda; 

	BasicGLS() { CreateParms(false); }
	~BasicGLS() { delete Parms; }

	inline void CreateParms(bool deleteOld = true) { if (deleteOld) delete Parms; Parms = new GLSParms(); }

	inline AlgoParms *GetParms() { return Parms; }
	
	inline void PreRun(Runner& runner)
	{
		n = Problem->Size;
		Parms->Calculate(n);
		Best = new Solution(runner.Problem);
		Best->GetFitness();
		Current = new Solution(*Best);  // current solution
		Penalty = Global::CreateMatrix(n, 0);  // Penalty values of all assignments
		Delta = Global::CreateMatrix(n);
		Current->SwapCostMatrix(Delta);
		lambda = GetLambda();
	}
	inline void PostRun()
	{
		delete Best;
		delete Current;
		Global::DeleteMatrix(Penalty, n);
		Global::DeleteMatrix(Delta,n);
	}

	inline double GetLambda()
	{
		int n = Problem->Size;
		double a=0, b=0;
		
		for (int i=0; i<n; ++i)
			for (int j=0; j<n; ++j)
			{
				a += Problem->Flow[i][j];
				b += Problem->Distance[i][j];
			}
		return a*b / pow(n,4) * Parms->Lambda;
	}

	inline double Iterate(Runner& runner)
	{
		LocalSearch(runner);
		UpdatePenalties();
		return Best->GetFitness();
	}


	inline void UpdatePenalties()
	{
		double costOfFeatureI, utility;
		int iBest; 
		double max = Global::Min;
		for (int i=0; i<n; ++i)
		{
			costOfFeatureI = 0;
			for (int j=0; j<n; ++j)
				costOfFeatureI += Problem->Distance[i][j] * Problem->Flow[(*Current)[i]][(*Current)[j]];
			utility = costOfFeatureI / (1 + Penalty[i][(*Current)[i]]);
			if (utility > max)
			{
				max = utility;
				iBest = i;
			}
		}
		++Penalty[iBest][(*Current)[iBest]];
	}

	// Find the deepest point based on augmented function h = g + penalties.  
	// returns true if new global best solution found.
	inline void LocalSearch(Runner& runner)
	{
		int sideCount = 0;
		int iBest, jBest;
		double min, cost;
		int lastSteepestDescentIteration = 0;
		do
		{
			min = Global::Max;

			for (int i = 0; i < n-1; ++i) 
				for (int j = i+1; j < n; ++j)
				{
					cost = Delta[i][j] + lambda * (-Penalty[i][(*Current)[i]] - Penalty[j][(*Current)[j]] + Penalty[i][(*Current)[j]] + Penalty[j][(*Current)[i]]);
					if (cost < min || cost == min && Global::Rand(2)==0) // ties are broken randomly
					{
						iBest = i; jBest = j; min = cost;
					}
				}

			if (min <= 0)
			{
				Current->Swap(iBest, jBest, &Delta[iBest][jBest]);
				Current->UpdateSwapCostMatrix(Delta);	

				if (Current->GetFitness() < Best->GetFitness())
				{
					delete Best;
					Best = new Solution(*Current);
				}

				if (min == 0)
					++sideCount;
				else sideCount = 0;
			}
			
		} while (!runner.IsDone() && min <= 0 && sideCount < 2);  // the result of the local search should be h(x) minimized, regardless of aspiration criteria in effect.
	}

};

class MultiGLS : public Algorithm
{
	public:
	GLSParms *Parms;
	Solution *Best, **Current;
	int n, k;
	double **Penalty, ***Delta, **Swaps;
	double lambda; 
	int TotalSwaps;

	MultiGLS() { CreateParms(false); }
	~MultiGLS() { delete Parms; }


	inline void CreateParms(bool deleteOld = true) { if (deleteOld) delete Parms; Parms = new GLSParms(); }

	inline AlgoParms *GetParms() { return Parms; }
	
	inline void PreRun(Runner& runner)
	{
		assert(Parms->IsThreaded());
		k = Parms->Threads;
		n = Problem->Size;
		Parms->Calculate(n);
		Best = new Solution(runner.Problem);
		Best->GetFitness();
		Current = new Solution*[k];
		Delta = new double**[k]; 
		for (int i=0; i<k; ++i)
		{
			Current[i] = new Solution(*Best); // new solution for each thread.
			Delta[i] = Global::CreateMatrix(n);
			Current[i]->SwapCostMatrix(Delta[i]);
		}
		
		Penalty = Global::CreateMatrix(n, 0);  // Penalty values of all assignments
		Swaps = Global::CreateMatrix(n,0);  
		lambda = GetLambda();
		TotalSwaps = 0;
	}
	inline void PostRun()
	{
		delete Best;
		delete [] Current;
		Global::DeleteMatrix(Penalty, n);
		for (int i=0; i<k; ++i)
			Global::DeleteMatrix(Delta[i],n);
		delete [] Delta;
		Global::DeleteMatrix(Swaps,n);
	}

	inline double GetLambda()
	{
		int n = Problem->Size;
		double a=0, b=0;
		
		for (int i=0; i<n; ++i)
			for (int j=0; j<n; ++j)
			{
				a += Problem->Flow[i][j];
				b += Problem->Distance[i][j];
			}
		return a*b / pow(n,4) * Parms->Lambda;
	}

	inline double Iterate(Runner& runner)
	{
		for (int i=0; i<k; ++i)
		{
			LocalSearch(i, runner);
			UpdatePenalties(i);
		}

		return Best->GetFitness();
	}


	inline void UpdatePenalties(int thread)
	{
		double costOfFeatureI, utility;
		int iBest; 
		double max = Global::Min;
		Solution &p = *Current[thread];
		for (int i=0; i<n; ++i)
		{
			costOfFeatureI = 0;
			for (int j=0; j<n; ++j)
				costOfFeatureI += Problem->Distance[i][j] * Problem->Flow[p[i]][p[j]];
			utility = costOfFeatureI / (1 + Penalty[i][p[i]]);
			if (utility > max)
			{
				max = utility;
				iBest = i;
			}
		}
		++Penalty[iBest][p[iBest]];
	}

	// Find the deepest point based on augmented function h = g + penalties.  
	// returns true if new global best solution found.
	inline void LocalSearch(int thread, Runner& runner)
	{
		int sideCount = 0;
		int iBest, jBest;
		double min, cost;
		int lastSteepestDescentIteration = 0;
		Solution &p = *Current[thread];
		double **&d = Delta[thread];
		bool best, bestPool;
		do
		{
			min = Global::Max;
			best = bestPool = false;

			for (int i = 0; i < n-1; ++i) 
				for (int j = i+1; j < n; ++j)
				{
					cost = p.GetFitness() + d[i][j];
					best = Parms->IsAspireBest && cost < Best->GetFitness();
					if (best && !bestPool) { bestPool = true; min = Global::Max; sideCount=0; }
					
					if (!best && bestPool)
						continue; // skip cost test if we're in a pool and this swap doesn't belong in the pool.

					// augmented cost is the difference between the augmented functions before and after a swap (Delta H)
					if (bestPool)
						cost = d[i][j];
					else
						cost = d[i][j] + lambda * (-Penalty[i][p[i]] - Penalty[j][p[j]] + Penalty[i][p[j]] + Penalty[j][p[i]]);
					
					if (cost < min || cost == min && Global::Rand(2)==0) // ties are broken randomly
					{
						iBest = i; jBest = j; min = cost;
					}
				}

			if (min <= 0 || bestPool)
			{
				SwapCurrent(thread,iBest,jBest);

				if (min == 0)
					++sideCount;
				else sideCount = 0;
			}
			
		} while (!runner.IsDone() && (bestPool || min <= 0 && sideCount < 2));  // the result of the local search should be h(x) minimized, regardless of aspiration criteria in effect.
	}

		// Tabu search without iteration constrained aspiration criterion (no need because we're using a perturbation method).  
	inline bool TabuSearch(int thread, int swapsLeft, int tabuLength, Runner& runner)
	{
		bool authorized, aspired, alreadyAspired;
		double minDelta, iBest, jBest, r;
		int iTabu, jTabu; 
		bool improved = false;
		
		Solution &p = *Current[thread];
		double **&d = Delta[thread];
		while (swapsLeft > 0 && !runner.IsDone())
		{
			minDelta = iBest = jBest = Global::Max; // in case all moves are tabu 
		
			alreadyAspired = false;

			for (int i = 0; i < n-1; ++i) 
				for (int j = i+1; j < n; ++j)
				{
					authorized = Swaps[i][p[j]] <= TotalSwaps || Swaps[j][p[i]] <= TotalSwaps;
					aspired = p.GetFitness() + d[i][j] < Best->GetFitness();                
					
					if (aspired && !alreadyAspired || aspired && alreadyAspired && d[i][j] < minDelta || 
						!aspired && !alreadyAspired && d[i][j] < minDelta && authorized)
					{
						iBest = i; jBest = j; minDelta = d[i][j];
						if (aspired) 
							alreadyAspired = true;
					}
				}

		
			if (iBest != Global::Max) // All moves are not tabud this iteration.
			{
				// Forbid reverse move for a non-uniform random number of iterations (taillard update 3/13/2006)
				r = Global::Rand();
				iTabu = (int)(r*r*r*tabuLength);
				r = Global::Rand();
				jTabu = (int)(r*r*r*tabuLength);
				if (SwapCurrent(iBest, jBest, iTabu, jTabu))
					improved = true;
				--swapsLeft; // only decrement swapsLeft if we actually swap. 
			}
		}
		return improved;
	}

	inline bool SwapCurrent(int thread, int i, int j, int iPenalty=0, int jPenalty=0)
	{
		Solution &p = *Current[thread];
		double **&d = Delta[thread];

		p.Swap(i, j, &d[i][j]);
		++TotalSwaps;
		p.UpdateSwapCostMatrix(d);	
		Swaps[i][p[i]] = max<int>(Swaps[i][p[i]], TotalSwaps + iPenalty); 
		Swaps[j][p[j]] = max<int>(Swaps[j][p[j]], TotalSwaps + jPenalty);

		if (p.GetFitness() < Best->GetFitness())
		{
			delete Best;
			Best = new Solution(p);
			
			return true;
		}
		return false;
	}

};

template<typename T, typename Container=std::vector<T> >
class PriorityQueue : public std::priority_queue<T,Container>
{
public:
    typedef typename Container::iterator iterator;
    typedef typename Container::const_iterator const_iterator;
    iterator begin() { return this->c.begin(); }
    iterator end() { return this->c.end(); }
    const_iterator begin() const { return this->c.begin(); }
    const_iterator end() const { return this->c.end(); }
};

class GLS : public Algorithm
{
public:
	GLSParms *Parms;
	Solution *Best, *Current;
	int n, TotalSwaps, LastSteepestDescent, LastBestMove, UniqueLocalSearches, LastUniqueLocalSearches, SwapsSinceImprovement;
	double **Penalty, **Delta, **Swaps, *Buffer;
	double LambdaBaseSize, CurrentEvaporateSinceImproveScale; 
	PriorityQueue<double> *BestSolutions;

	// Steep GLS Distance Mutation
	int CurrentDistanceFromReference;
	Solution *Reference;
	
	GLS() { CreateParms(false); }
	~GLS() { delete Parms; }

	inline void CreateParms(bool deleteOld = true) { if (deleteOld) delete Parms; Parms = new GLSParms(); }
	inline AlgoParms *GetParms() { return Parms; }
	
	inline void PreRun(Runner& runner)
	{
		TotalSwaps = 0; // number of swaps performed on current solution since the beginning. 
		n = Problem->Size;
		Parms->Calculate(n);
		Best = new Solution(runner.Problem);
		Best->GetFitness();
		Current = new Solution(*Best);  // current solution
		Penalty = Global::CreateMatrix(n, 0);  // Penalty values of all assignments
		Swaps = Global::CreateMatrix(n, 0);  // History of Swaps.
		Delta = Global::CreateMatrix(n);
		Buffer = new double[n]; 
		Current->SwapCostMatrix(Delta);
		LambdaBaseSize = GetLambdaBaseSize();
		LastSteepestDescent = LastBestMove = UniqueLocalSearches = SwapsSinceImprovement = 0;
		CurrentDistanceFromReference = 0;
		Reference = NULL;
		BestSolutions = new PriorityQueue<double>();
		BestSolutions->push(Best->GetFitness());
		if (Parms->IsEvaporateSinceImprove())
			CurrentEvaporateSinceImproveScale = Parms->EvaporateSinceImproveScale;
	}
	inline void PostRun()
	{
		delete Best;
		delete Current;
		delete Reference;
		Global::DeleteMatrix(Penalty, n);
		Global::DeleteMatrix(Swaps,n);
		Global::DeleteMatrix(Delta,n);
		delete [] Buffer;
		delete BestSolutions;
	}

	inline double GetLambdaBaseSize()
	{
		int n = Problem->Size;
		double a=0, b=0;
		
		for (int i=0; i<n; ++i)
			for (int j=0; j<n; ++j)
			{
				a += Problem->Flow[i][j];
				b += Problem->Distance[i][j];
			}
		return a*b / pow(n,4);
	}

	inline double Lambda()  
	{
		double lambda = Parms->Lambda;
		if (Parms->IsDynamicLambda())
			lambda = Parms->Lambda * (1-pow(Global::Rand(),Parms->LambdaPower)); // non-uniform random values closer to lambda are generated  

		return lambda * LambdaBaseSize; 
	}

	inline double Iterate(Runner& runner)
	{
		if (!Parms->IsSteepMode())
		{
			LastUniqueLocalSearches = UniqueLocalSearches;
			LocalSearch(runner);
			if (Parms->IsSteepestDescentAlways)
				SteepestDescent(runner);	
			UpdatePenalties();
			
			if (Parms->IsEvaporateOnInterval() && UniqueLocalSearches!=LastUniqueLocalSearches && UniqueLocalSearches % Parms->evaporateInterval == 0) // Mod ever uniqueLocalSearchCount interval because in some instances (tai-a) there are on average 6 penalty increases before the call to LocalSearch actually causes a move (swap).
				ReducePenaltiesBy(Parms->EvaporateOnIntervalScale);
		}
		else // STEEP MODE
		{
			// Default to steepest descent, and use limited but increasing swaps of GLS as variable perturbation mechanism to escape local optima/basins, then restart
			if (!Parms->Steep.IsTabuSearch())
			{
				if (SteepestDescent(runner))
					Parms->Steep.ResetPerturb();
			}
			else 
			{
				if (TabuSearch(runner, Parms->Steep.tabuSearchSwaps, Parms->Steep.tabuLength)) 
					Parms->Steep.ResetPerturb();
			}

			UpdatePenalties();

			if (!Parms->Steep.IsMutateByDistance)
			{
				int swapsLeft = Parms->Steep.perturb;
				Parms->Steep.IncreasePerturb();
				while (swapsLeft > 0 && !runner.IsDone())
				{
					if (LocalSearch(runner, swapsLeft)) // Reached augmented local optimum.
					{
						Parms->Steep.ResetPerturb();
						break; // improved best solution so stop any remaining perturbations immediately!
					}
					if (swapsLeft > 0)
						UpdatePenalties(); // Modify landscape only if need to continue augmented steepest descent local search (because haven't fully perturbed yet).  
				}
			}
			else // Run GLS until we are truly the distance away from the reference point.
			{
				CurrentDistanceFromReference = 0;
				Reference = new Solution(*Current);
				Parms->Steep.IncreasePerturb();
				while (CurrentDistanceFromReference < Parms->Steep.perturb && !runner.IsDone())
				{
					if (LocalSearch(runner))
					{
						Parms->Steep.ResetPerturb();
						break; // improved best solution so stop any remaining perturbations immediately!
					}
					if (CurrentDistanceFromReference < Parms->Steep.perturb)
						UpdatePenalties();
				}
				delete Reference;
				Reference = NULL;

			}

		}
		return Best->GetFitness();
	}


	inline void UpdatePenalties()
	{
		int penalizedIndex = -1;
		switch (Parms->penaltyMode)
		{
			case IncreaseByUtility: penalizedIndex = AdjustSinglePenalty(true,CalculateUtility()); break;
			case IncreaseAllByUtility: AdjustAllPenalty(true,CalculateUtility()); break;
			case IncreaseByCost: penalizedIndex = AdjustSinglePenalty(true,CalculateCost()); break;
			case IncreaseAllByCost: AdjustAllPenalty(true, CalculateCost()); break;
		}
		
		if (Parms->IsEvaporate())
		{
			switch (Parms->EvaporateMode)
			{
				case EvapByPenalty: AdjustSinglePenalty(false, CalculatePenalty(), penalizedIndex); break;
				case EvapAllByPenalty : AdjustAllPenalty(false, CalculatePenalty(), penalizedIndex); break;
				case EvapByLate: AdjustSinglePenalty(false, CalculateLate()); break;
				case EvapAllByLate: AdjustAllPenalty(false, CalculateLate()); break;
			}
		}
	}

	inline double* CalculateUtility() { return CalculateUtilityElseCost(true);}
	inline double* CalculateCost() { return CalculateUtilityElseCost(false); }
	inline double* CalculateUtilityElseCost(bool isUtility)
	{
		double costOfFeatureI;
		for (int i=0; i<n; ++i)
		{
			costOfFeatureI = 0;
			for (int j=0; j<n; ++j)
				costOfFeatureI += Problem->Distance[i][j] * Problem->Flow[(*Current)[i]][(*Current)[j]];
			Buffer[i] = costOfFeatureI / (isUtility ? 1 + Penalty[i][(*Current)[i]] : 1); // if !isUtility, just calculate the cost and no denominator
		}
		return Buffer;
	}

	inline double* CalculatePenalty()
	{
		for (int i=0; i<n; ++i)
			Buffer[i] = Penalty[i][(*Current)[i]];
		return Buffer;
	}
	
	inline double* CalculateLate()
	{
		for (int i=0; i<n; ++i)
			Buffer[i] = max(0.0,TotalSwaps - Swaps[i][(*Current)[i]]); // the larger this difference is the older it is
		return Buffer;
	}

	inline int AdjustSinglePenalty(bool increase, double costs[], int ignoreIndex=-1)
	{
		double maxx = Global::Min; 
		int iBest;
		for (int i=0; i<n; ++i)
		{
			if (i == ignoreIndex) continue;
			if (costs[i] > maxx || costs[i] == maxx && Global::Rand(2)==0) 
			{
				maxx = costs[i];
				iBest = i;
			}
		}
		double &p = Penalty[iBest][(*Current)[iBest]];
		if (increase)
			p += Parms->penaltyAmount; 
		else p = max(0.0, p - Parms->EvaporateAmount); // No negatives allowed.
		return iBest;
	}

	inline void AdjustAllPenalty(bool increase, double costs[], int ignoreIndex=-1)
	{
		double sum = 0;
		if (ignoreIndex >=0 && ignoreIndex < n) 
			costs[ignoreIndex] = 0;
		for (int i=0; i<n; ++i)
			sum += costs[i];
		if (sum == 0) // No divide by zero allowed!
			return;
		for (int i=0; i<n; ++i)
		{
			double &p = Penalty[i][(*Current)[i]];
			if (increase)
				p += costs[i]/sum * Parms->penaltyAmount;
			else
				p = max(0.0, p - costs[i] / sum  * Parms->EvaporateAmount);  // No negatives allowed.   
		}
	}

	// Find the deepest point based on augmented function h = g + penalties.  
	// returns true if new global best solution found.
	inline bool LocalSearch(Runner& runner, int& swapsLeft=Global::Max)
	{
		int sideCount = 0;
		int iBest, jBest;
		double min, cost;
		bool isSwap, bestPool, latePool, best, good, goodPool, late, original, randMove, bestMove;
		bool improvedBest = false;
		// DOuble check if a variable initialized here should be initialized class wide instead.

		int startSwap = TotalSwaps;

		if (swapsLeft <= 0) return false;

		do
		{
			min = Global::Max;
			best = bestPool = late = latePool = good = goodPool = original = randMove = bestMove = false;

			if (Parms->IsRandomMove() && Global::Rand() < Parms->RandomMovePr)
			{
				randMove = true;
				iBest = Global::Rand(n); 
				while ((jBest=Global::Rand(n))==iBest);
			}
			else if (Parms->IsBestMoveInterval() && TotalSwaps-LastBestMove > Parms->bestMoveInterval
				  || Parms->IsBestMovePr() && Global::Rand() < Parms->BestMovePr) 
			{
				LastBestMove = TotalSwaps;
				bestMove = true;
				for (int i = 0; i < n-1; ++i) 
					for (int j = i+1; j < n; ++j)
						if (Delta[i][j] < min)
						{
							iBest = i; jBest = j; min = Delta[i][j];
						}
			}
			else
			{
				for (int i = 0; i < n-1; ++i) 
					for (int j = i+1; j < n; ++j)
					{
						cost = Current->GetFitness() + Delta[i][j];
						best = Parms->IsAspireBest && cost < Best->GetFitness();
						if (best && !bestPool) { bestPool = true; min = Global::Max; sideCount=0; }
					
						good = false;
						if (Parms->IsAspireGood() && !bestPool)
						{
							if (cost < BestSolutions->top() || BestSolutions->size() < Parms->aspireGood) // do not aspireGood if the fitness already equals one of the good list fitnesses -- we don't want to allow cycles and repeat solutions. 
							{
								//vector<double> *v = reinterpret_cast<vector<double> *>(&BestSolutions); // cast to the priority queue base vector to allow iteration.  
								bool found = false;
								for (PriorityQueue<double>::iterator i=BestSolutions->begin(); i!=BestSolutions->end(); ++i)
									if (cost == *i) { found = true; break; }
								good = !found;
							}
						}
						if (good && !goodPool) { goodPool = true; min = Global::Max; sideCount=0; } 

						late = Parms->IsAspireLate() && !bestPool && !goodPool && (Swaps[i][(*Current)[j]] < TotalSwaps - Parms->aspireLate || Swaps[j][(*Current)[i]] < TotalSwaps - Parms->aspireLate);
						if (late && !latePool) { latePool = true; min = Global::Max; sideCount=0; }

						original = !bestPool && !latePool && !goodPool;
						if (!best && bestPool || !good && goodPool && !bestPool || !late && latePool && !bestPool && !goodPool)
							continue; // skip cost test if we're in a pool and this swap doesn't belong in the pool.

						// augmented cost is the difference between the augmented functions before and after a swap (Delta H)
						if (bestPool || latePool || goodPool)
							cost = Delta[i][j];
						else if (Parms->IsPenaltyNoise())
							cost = Delta[i][j] + Lambda() * (-Penalty[i][(*Current)[i]] - Penalty[j][(*Current)[j]] + (1 - Global::Rand()*Parms->PenaltyNoisePr)*(Penalty[i][(*Current)[j]] + Penalty[j][(*Current)[i]]));
						else
							cost = Delta[i][j] + Lambda() * (-Penalty[i][(*Current)[i]] - Penalty[j][(*Current)[j]] + Penalty[i][(*Current)[j]] + Penalty[j][(*Current)[i]]);
					
						if (cost < min || cost == min && Global::Rand(2)==0) // ties are broken randomly
						{
							iBest = i; jBest = j; min = cost;
						}
					}
			}

			if (min <= 0 && original || bestPool || latePool || goodPool || randMove || bestMove && min < 0)
			{
				if (SwapCurrent(iBest, jBest))
				{
					improvedBest = true;
					if (Parms->IsEvaporateOnImprove())
						ReducePenaltiesBy(Parms->EvaporateOnImproveScale);
				}

				if (Parms->IsAspireGood() && (bestPool || goodPool))
				{
					if (BestSolutions->size() == Parms->aspireGood)
						BestSolutions->pop(); // pop worst of best solutions off to make room for the new one.
					BestSolutions->push(Current->GetFitness());
				}
						

				if (Parms->IsEvaporateOnSwap())
					ReducePenaltiesBy(Parms->EvaporateOnSwapScale);

				--swapsLeft;

				if (min == 0)
					++sideCount;
				else sideCount = 0;

				if (Parms->IsSteepestDescentInterval() && (original || min < 0) && TotalSwaps - LastSteepestDescent > Parms->steepestDescentInterval)
				{
					if (SteepestDescent(runner))	
						improvedBest = true; 
					LastSteepestDescent = TotalSwaps;
				}
			}
			
		} while (!runner.IsDone() && swapsLeft > 0 && (!original || original && min <= 0 && sideCount < 2));  // the result of the local search should be h(x) minimized, regardless of aspiration criteria/extension in effect.

		if (TotalSwaps-startSwap > 0) ++UniqueLocalSearches;

		return improvedBest;
	}

	// Reduce all penalties by scale*100 percent.  
	inline void ReducePenaltiesBy(double scale)
	{
		if (scale <= 0) return;
		if (scale > 1.0) scale = 1.0;
		for (int i=0; i<n; ++i) 
			for (int j=0; j<n; ++j) 
				Penalty[i][j] *= (1-scale);
	}

	// Minimize g(x)  (Find the deepest point based on original function g)  
	// returns true if best solution improved.
	inline bool SteepestDescent(Runner& runner)
	{
		int iBest, jBest;
		double min;
		bool improvedBest = false;
		do
		{
			min = Global::Max;
			for (int i = 0; i < n-1; ++i)
				for (int j = i+1; j < n; ++j)
					if (Delta[i][j] < min)
					{
						iBest = i; jBest = j; min = Delta[i][j];
					}
			if (min < 0)
			{
				if (SwapCurrent(iBest, jBest))
					improvedBest = true;
			}
		} while (min < 0 && !runner.IsDone()); 
		return improvedBest;
	}

	// Tabu search without iteration constrained aspiration criterion (no need because we're using a perturbation method).  
	inline bool TabuSearch(Runner& runner, int swapsLeft, int tabuLength)
	{
		bool authorized, aspired, alreadyAspired;
		double minDelta, iBest, jBest, r;
		int iTabu, jTabu; 
		bool improved = false;
		while (swapsLeft > 0 && !runner.IsDone())
		{
			minDelta = iBest = jBest = Global::Max; // in case all moves are tabu 
		
			alreadyAspired = false;

			Solution &p = *Current;
			for (int i = 0; i < n-1; ++i) 
				for (int j = i+1; j < n; ++j)
				{
					authorized = Swaps[i][p[j]] <= TotalSwaps || Swaps[j][p[i]] <= TotalSwaps;
					aspired = p.GetFitness() + Delta[i][j] < Best->GetFitness();                
					
					if (aspired && !alreadyAspired || aspired && alreadyAspired && Delta[i][j] < minDelta || 
						!aspired && !alreadyAspired && Delta[i][j] < minDelta && authorized)
					{
						iBest = i; jBest = j; minDelta = Delta[i][j];
						if (aspired) 
							alreadyAspired = true;
					}
				}

		
			if (iBest != Global::Max) // All moves are not tabud this iteration.
			{
				// Forbid reverse move for a non-uniform random number of iterations (taillard update 3/13/2006)
				r = Global::Rand();
				iTabu = (int)(r*r*r*tabuLength);
				r = Global::Rand();
				jTabu = (int)(r*r*r*tabuLength);
				if (SwapCurrent(iBest, jBest, iTabu, jTabu))
					improved = true;
				--swapsLeft; // only decrement swapsLeft if we actually swap. 
			}
		}
		return improved;
	}

	inline bool SwapCurrent(int i, int j, int iPenalty=0, int jPenalty=0)
	{
		// Iteratively calculate distance from Reference solution.
		if (Parms->Steep.IsMutateByDistance && Reference != NULL)
		{
			Solution &r = *Reference;
			Solution &c = *Current;
			if (c[j] == r[j]) ++CurrentDistanceFromReference; // no matter what the new c[j] will be different after we swap i,j.  
			if (c[i] == r[i]) ++CurrentDistanceFromReference; // no matter what the new c[i] will be different after we swap i,j.  
			if (c[i] == r[j]) --CurrentDistanceFromReference; 
			if (c[j] == r[i]) --CurrentDistanceFromReference; 
		}

		Current->Swap(i, j, &Delta[i][j]);
		++TotalSwaps;
		Current->UpdateSwapCostMatrix(Delta);	
		Swaps[i][(*Current)[i]] = max<int>(Swaps[i][(*Current)[i]], TotalSwaps + iPenalty); 
		Swaps[j][(*Current)[j]] = max<int>(Swaps[j][(*Current)[j]], TotalSwaps + jPenalty);

		if (Current->GetFitness() < Best->GetFitness())
		{
			delete Best;
			Best = new Solution(*Current);
			SwapsSinceImprovement = 0;
			return true;
		}
		
		++SwapsSinceImprovement;
		if (Parms->IsEvaporateSinceImprove() && SwapsSinceImprovement != 0 && SwapsSinceImprovement % Parms->evaporateSinceImproveInterval == 0)
		{
			ReducePenaltiesBy(CurrentEvaporateSinceImproveScale);
			CurrentEvaporateSinceImproveScale *= Parms->EvaporateSinceImproveDecay;
		}
		return false;
	}
};


class TabuSearch : public Algorithm
{
public:
	TabuSearchParms *Parms;
	Solution *Best, *Current;
	int n;
	double **tabuList, **delta;


	bool authorized;  // move not tabu?
	bool aspired;     // move forced?
	bool alreadyAspired;  // In case many moves forced
	int iBest, jBest;
	double minDelta;
	double r;
	int run, tabu; // How many runs has there been since an improvement was found.
	int aspireCount;

	TabuSearch() { CreateParms(false); }
	~TabuSearch() { delete Parms; }


	inline void CreateParms(bool deleteOld = true) { if (deleteOld) delete Parms; Parms = new TabuSearchParms(); }
	inline AlgoParms *GetParms() { return Parms; }
	inline void PreRun(Runner& runner)
	{
		n = Problem->Size;
		Parms->Calculate(n); 

		Best = new Solution(runner.Problem);
		Best->GetFitness();
		Current = new Solution(*Best);  // current solution

		tabuList = Global::CreateMatrix(n);  // Tabu status
		for (int i = 0; i < n; ++i) 
			for (int j = 0; j < n; ++j)
				tabuList[i][j] = -(n*i + j);	
		
		delta = Global::CreateMatrix(n);
		Current->SwapCostMatrix(delta);
		aspireCount = 0;
	}

	inline void PostRun()
	{
		delete Best;
		delete Current;
		Global::DeleteMatrix(delta, n);
		Global::DeleteMatrix(tabuList, n);
	}

	inline double Iterate(Runner& runner)
	{
		run = runner.Iteration;	

		// Find best move (iBest, jBest) 
		minDelta = iBest = jBest = Global::Max; // in case all moves are tabu 
		alreadyAspired = false;

		Solution &p = *Current;
		for (int i = 0; i < n-1; ++i) 
			for (int j = i+1; j < n; ++j)
			{
				authorized = tabuList[i][p[j]] < run || tabuList[j][p[i]] < run;
				aspired = tabuList[i][p[j]] < run - Parms->t || tabuList[j][p[i]] < run - Parms->t || p.GetFitness() + delta[i][j] < Best->GetFitness();                
					
				if (aspired && !alreadyAspired || aspired && alreadyAspired && delta[i][j] < minDelta || 
					!aspired && !alreadyAspired && delta[i][j] < minDelta && authorized)
				{
					iBest = i; jBest = j; minDelta = delta[i][j];
					if (aspired) 
						alreadyAspired = true;
				}
			}

		
		if (iBest != Global::Max) // All moves are not tabud this iteration.
		{
			// Swap elements in pos. iBest and jBest
			p.Swap(iBest, jBest, &minDelta);
			
			// Forbid reverse move for a non-uniform random number of iterations (taillard update 3/13/2006)
			r = Global::Rand();
			tabuList[iBest][p[jBest]] = run + (int)(r*r*r*Parms->u);
			r = Global::Rand();
			tabuList[jBest][p[iBest]] = run + (int)(r*r*r*Parms->u);
			
			if (p.GetFitness() < Best->GetFitness())
			{
				delete Best;
				Best = new Solution(*Current);
			}

			// Update matrix of move costs
			Current->UpdateSwapCostMatrix(delta);
		}
	
		return Best->GetFitness();
	}

};






enum Phase { Work, Core, Exploit, Explore };

class MyTabuSearch : public Algorithm
{
public:
	MyTabuSearchParms *Parms;
	Solution *Best, *Current;
	int n;
	double **tabuList, **delta;


	bool authorized;  // move not tabu?
	bool aspired, pureAspired;     // move forced?
	bool alreadyAspired;  // In case many moves forced
	int iBest, jBest;
	double minDelta;
	double r;
	int run, failedRuns, tabu; // How many runs has there been since an improvement was found.
	int aspireCount;
	Phase phase;

	MyTabuSearch() { CreateParms(false); }
	~MyTabuSearch() { delete Parms; }

	inline void CreateParms(bool deleteOld = true) { if (deleteOld) delete Parms; Parms = new MyTabuSearchParms(); }
	inline AlgoParms *GetParms() { return Parms; }

	inline void PreRun(Runner& runner)
	{
		n = Problem->Size;
		Parms->Calculate(n); 

		Best = new Solution(runner.Problem);
		Best->GetFitness();
		Current = new Solution(*Best);  // current solution

		tabuList = Global::CreateMatrix(n);  // Tabu status
		for (int i = 0; i < n; ++i) 
			for (int j = 0; j < n; ++j)
				tabuList[i][j] = -(n*i + j);	
		
		delta = Global::CreateMatrix(n);
		Current->SwapCostMatrix(delta);
		failedRuns = 0;
		aspireCount = 0;
		phase = Explore;
	}

	inline void PostRun()
	{
		delete Best;
		delete Current;
		Global::DeleteMatrix(delta, n);
		Global::DeleteMatrix(tabuList, n);
	}

	inline double Iterate(Runner& runner)
	{
		run = runner.Iteration;	

		// JOLT every JOLTRATE runs
		if (Parms->Jolt != NULL && failedRuns != 0 && failedRuns%Parms->joltRate == 0)
		{
			if (Parms->JoltBest)
			{
				delete Current;
				Current = new Solution(*Best);
			}

			r = Current->GetFitness();
			Parms->Jolt->Enhance(*Current, run);
			if (r != Current->GetFitness() || Parms->JoltBest) // if jolt actually generated a new solution
				Current->SwapCostMatrix(delta); // Recalculate entire swapmatrix because a swap[s] just occurred from jolting.
		}
		
		// Constructor
		if (Parms->Constructor != NULL && failedRuns != 0 && failedRuns%Parms->constructRate == 0)
		{
			delete Current;
			Current = Parms->Constructor->Generate(*Problem);
			Current->SwapCostMatrix(delta);
		}


		// Reactive u
		if (Parms->UMax != NULL)
		{
			if (failedRuns == 0) // Just improved
			{
				Parms->u = Parms->uMin; // Reset u to min.
			}
			if (failedRuns != 0 && failedRuns%Parms->increaseRate == 0)
			{
				// No improvement for so long, so it's time to increase u!
				Parms->u = min(Parms->u + Parms->deltaU, Parms->uMax);
			}
		}

		// Reactive t
		if (Parms->TMax != NULL && run != 0 && run % Parms->tIncreaseRate == 0)
		{
			if (Parms->t == Parms->tMax)
			{
				if (Parms->ScopeTS())
				{
					Parms->tMin = Parms->tMinBest;
					Parms->tMax = Parms->tMaxBest;
					Parms->tAvgBest = Global::Max;
					Parms->scopeValues.clear();
					Parms->scopeTs.clear();
					Parms->scopeRatio -= (Parms->scopeRatio - Parms->ScopeMin) * Parms->ScopeRate;
					Parms->scope = max(1,int(Parms->scopeRatio * Parms->tDelta + .5));
					Parms->scopeSum = 0;
				}
				if (Parms->TIncreasePercentMax != 0)
				{
					Parms->tIncreasePercent += (Parms->TIncreasePercentMax - Parms->tIncreasePercent) * Parms->TIncreasePercentRate;
					Parms->tIncrease = max(1, int((Parms->tMax-Parms->tMin)*Parms->tIncreasePercent+.5));
				}

				Parms->t = Parms->tMin;
			}
			else
				Parms->t = min(Parms->t + Parms->tIncrease, Parms->tMax);

			if (Parms->UAuto != NULL)
				Parms->u = Parms->UAuto->Calculate(Parms->t, n);
		}		

		// Exploit TS
		if (Parms->Exploit != NULL)
		{
			if (phase == Explore)
			{
				if (aspireCount != 0)
					Parms->exploreCurrent = 0;
				else if (aspireCount == 0 && run > 0)
					++Parms->exploreCurrent;
				if (Parms->exploreCurrent > Parms->explore)
				{
					phase = Exploit;
					Parms->exploitBest = Global::Max;
					Parms->exploitCurrent = 0;
				}
			}
			else if (phase == Exploit)
			{
				if (Current->GetFitness() < Parms->exploitBest)
				{
					Parms->exploitBest = Current->GetFitness();
					Parms->exploitCurrent = 0;
				}
				else 
					++Parms->exploitCurrent;
				
				if (Parms->exploitCurrent > Parms->exploit)
				{
					phase = Explore;
					Parms->exploreCurrent = 0;
				}
			}
		}



		// Find best move (iBest, jBest) 
		iBest = Global::Max; // in case all moves are tabu
		jBest = Global::Max;
		minDelta = Global::Max;
		alreadyAspired = false;

		aspireCount = 0;

		for (int i = 0; i < n-1; ++i) 
			for (int j = i+1; j < n; ++j)
			{
				authorized = tabuList[i][(*Current)[j]] < run || tabuList[j][(*Current)[i]] < run;
				pureAspired = tabuList[i][(*Current)[j]] < run - Parms->t || tabuList[j][(*Current)[i]] < run - Parms->t;
				
				if (Parms->Exploit != NULL && phase == Exploit && pureAspired) // In exploit phase => ignore any pure aspirations and defer aspiring until later.
				{
					pureAspired = false;
					if (tabuList[i][(*Current)[j]] < run - Parms->t) 
						tabuList[i][(*Current)[j]] = min((int)tabuList[i][(*Current)[j]] + Parms->defer, run); // do not defer past current iteration! We do not want to forbid access to it!
					if (tabuList[j][(*Current)[i]] < run - Parms->t)
						tabuList[j][(*Current)[i]] = min((int)tabuList[j][(*Current)[i]] + Parms->defer, run); // do not defer past current iteration! We do not want to forbid access to it!
				}
				aspired = pureAspired || Current->GetFitness() + delta[i][j] < Best->GetFitness();                
				
				if (aspired && !alreadyAspired || aspired && alreadyAspired && delta[i][j] < minDelta || 
					!aspired && !alreadyAspired && delta[i][j] < minDelta && authorized)
				{
					iBest = i; jBest = j; minDelta = delta[i][j];
					if (aspired) 
						alreadyAspired = true;
				}

				if (pureAspired) 
					++aspireCount;
			}

		if (iBest != Global::Max) // All moves are not tabud this iteration.
		{

			// Swap elements in pos. iBest and jBest
			Current->Swap(iBest, jBest, &minDelta);
			
			// Forbid reverse move for a non-uniform random number of iterations (taillard update 3/13/2006)
			r = Global::Rand();
			tabu = run + (int)(r*r*r*Parms->u);
			tabuList[iBest][ (*Current)[jBest] ] = tabu; 
			
			r = Global::Rand();
			tabu = run + (int)(r*r*r*Parms->u);
			tabuList[jBest][ (*Current)[iBest] ] = tabu;

			// Update frequency matrix 
			if (Parms->Constructor != NULL)
			{
				Parms->Constructor->UpdateFrom(*Current, iBest);
				Parms->Constructor->UpdateFrom(*Current, jBest); 
			}

			// Best solution improved ?
			if (Current->GetFitness() < Best->GetFitness())
			{
				Best = new Solution(*Current);
				failedRuns = 0;
			}
			else   
				++failedRuns; 

			// Update matrix of move costs
			Current->UpdateSwapCostMatrix(delta);
		}


		if (Parms->ScopeTS())
		{
			Parms->scopeValues.push_front(Current->GetFitness());
			Parms->scopeTs.push_front(Parms->t);
			Parms->scopeSum += Parms->scopeValues.front();
			if (Parms->scopeValues.size() > Parms->scope)
			{
				Parms->scopeSum -= Parms->scopeValues.back();
				Parms->scopeValues.pop_back();
				Parms->scopeTs.pop_back();
				if (Parms->scopeSum/Parms->scope < Parms->tAvgBest)
				{
					Parms->tAvgBest = Parms->scopeSum / Parms->scope;
					Parms->tMinBest = Parms->scopeTs.back();
					Parms->tMaxBest = Parms->scopeTs.front();
				}
			}
		}


		// Debug
		if (Parms->Debug)
		{
			stringstream ss;
			if (Parms->Exploit != NULL)
			{
				ss << aspireCount << ", " << Current->GetDeviation() << endl;
				Debug(ss.str());
			}
			else if (Parms->TMax != NULL)
			{
				ss << run << ", " << Parms->t << ", " << Current->GetDeviation() << endl;
				Debug(ss.str());
			}
		}
		return Best->GetFitness();
	}



};


class CoreTS : public Algorithm
{
public:
	MyTabuSearchParms *Parms;
	Solution *Best, *Current;
	int n;
	double **tabuList, **delta;


	bool authorized;  // move not tabu?
	bool aspired;     // move forced?
	bool alreadyAspired;  // In case many moves forced
	int iBest, jBest;
	double minDelta;
	double r;
	Phase phase;
	int run, failedRuns, tabu; // How many runs has there been since an improvement was found.

	bool isCoreTabu;

	CoreTS() { CreateParms(false); }
	~CoreTS() { delete Parms; }


	inline void CreateParms(bool deleteOld = true) { if (deleteOld) delete Parms; Parms = new MyTabuSearchParms(); }
	inline AlgoParms *GetParms() { return Parms; }
	inline void PreRun(Runner& runner)
	{
		n = Problem->Size;
		Parms->Calculate(n); 

		Best = new Solution(runner.Problem);
		Best->GetFitness();
		Current = new Solution(*Best);  // current solution

		tabuList = Global::CreateMatrix(n);  // Tabu status
		for (int i = 0; i < n; ++i) 
			for (int j = 0; j < n; ++j)
				tabuList[i][j] = -(n*i + j);	
		
		delta = Global::CreateMatrix(n);
		Current->SwapCostMatrix(delta);
		failedRuns = 0;
		phase = Work;
	}

	inline void PostRun()
	{
		delete Best;
		delete Current;
		Global::DeleteMatrix(delta, n);
		Global::DeleteMatrix(tabuList, n);
	}

	inline double Iterate(Runner& runner)
	{
		run = runner.Iteration;	

		// Core Construction
		if (Parms->CoreSize != NULL)
		{
			int remainder = run%(Parms->coreWorkPhaseLength + Parms->corePhaseLength);
			if (remainder == Parms->coreWorkPhaseLength)
			{
				phase = Core;
				//delete Current;
				//Current = Parms->CoreConstructor->Generate(*Problem, Parms->coreSize);
				//Current->SwapCostMatrix(delta);
				// Tabu core components
				
				// calculate core, delete Construction result
				delete Parms->CoreConstructor->Generate(*Problem, Parms->coreSize);
				if (Parms->MaxCoreSize != NULL)
				{
					Parms->coreSize += Parms->coreIncrease;
					if (Parms->coreSize > Parms->maxCoreSize)
						Parms->coreSize = Parms->minCoreSize;
				}
			}
			else if (remainder == 0)
			{
				phase = Work;
			}
		}

		// Find best move (iBest, jBest) 
		iBest = Global::Max; // in case all moves are tabu 
		jBest = Global::Max;
		minDelta = Global::Max;
		alreadyAspired = false; 

		for (int i = 0; i < n-1; ++i) 
		{
			if (Parms->CoreSize != NULL && phase == Core && Parms->CoreConstructor->IsInCore(i, (*Current)[i])) // Component at i is part of the core, so skip this i.
				continue;

			for (int j = i+1; j < n; ++j)
			{
				if (Parms->CoreSize != NULL && phase == Core && Parms->CoreConstructor->IsInCore(j, (*Current)[j])) // Component at j is part of the core, so skip this j.
					continue;
				
				authorized = tabuList[i][(*Current)[j]] < run || tabuList[j][(*Current)[i]] < run;
				aspired = tabuList[i][(*Current)[j]] < run - Parms->t || tabuList[j][(*Current)[i]] < run - Parms->t || Current->GetFitness() + delta[i][j] < Best->GetFitness();                
					
				if (aspired && !alreadyAspired || aspired && alreadyAspired && delta[i][j] < minDelta || 
					!aspired && !alreadyAspired && delta[i][j] < minDelta && authorized)
				{
					iBest = i; jBest = j; minDelta = delta[i][j];
					if (aspired) 
						alreadyAspired = true;
				}
			}
		}

		if (iBest == Global::Max) 
			return Best->GetFitness(); //  All moves are tabu this iteration!

		// Swap elements in pos. iBest and jBest
		Current->Swap(iBest, jBest, &minDelta);
			
		// Forbid reverse move for a non-uniform random number of iterations (taillard update 3/13/2006)
		r = Global::Rand();
		tabu = run + (int)(r*r*r*Parms->u);
		tabuList[iBest][ (*Current)[jBest] ] = tabu; 
			
		r = Global::Rand();
		tabu = run + (int)(r*r*r*Parms->u);
		tabuList[jBest][ (*Current)[iBest] ] = tabu;

		if (Parms->CoreSize != NULL && phase == Work)
		{
			Parms->CoreConstructor->UpdateFrom(*Current, iBest);
			Parms->CoreConstructor->UpdateFrom(*Current, jBest);
		}
			
		// Best solution improved ?
		if (Current->GetFitness() < Best->GetFitness())
		{
			Best = new Solution(*Current);
			failedRuns = 0;
		}
		else  
			++failedRuns; 

		// Update matrix of move costs
		Current->UpdateSwapCostMatrix(delta);

		return Best->GetFitness();
	}



};
