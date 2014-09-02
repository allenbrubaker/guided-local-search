#pragma once
#include "Solution.cpp"
#include <float.h>
#include "Global.cpp"
#include <limits.h>
#include <typeinfo>
#include <sstream>
#include <algorithm>
#include <vector>
#include "Ratio.cpp"
#include "Runner.cpp"
using namespace std;

class LocalSearch
{
public:
	virtual void Enhance(Solution& solution, int globalIteration, int iterations=Global::Max, Runner* runner = NULL) = 0;

	inline string Name() 
	{
		string name = string(typeid(*this).name());
		int chop = getenv("windir") != NULL ? 6 : 1;
		return name.substr(chop,name.size()-chop);
	}
	inline string ToString()
	{
		string s = ParmsToString();
		return "(" + Name() + (s=="" ? "" : ":") + s + ")";
	}
	inline virtual string ParmsToString() { return ""; }

};

class RoTS : public LocalSearch
{
public:
	Ratio *T, *U, *Runs, *JoltRate;
	bool ResetIfImprove, JoltBest;
	LocalSearch *Jolt;
	RoTS(Ratio* runs, bool resetIfImprove, Ratio* u, Ratio* t, LocalSearch *jolt=NULL, Ratio *joltRate=NULL, bool joltBest=0) : Runs(runs), ResetIfImprove(resetIfImprove), T(t), U(u), Jolt(jolt), JoltRate(joltRate), JoltBest(joltBest) {}

	~RoTS() {delete T; delete U; delete Runs; delete Jolt; delete JoltRate;}

	inline string ParmsToString()
	{
		stringstream s;
		s << "runs=" << Runs->ToString() << " imp=" << ResetIfImprove << " u=" << U->ToString() << " t=" << T->ToString();
		if (Jolt != NULL)
		{
			s << "jolt=" << Jolt->ToString() << " rate=" << JoltRate->ToString() << " best=" << JoltBest;
		}
		return s.str();
	}

	void Enhance(Solution& best, int globalIteration,  int iterations=Global::Max, Runner* runner = NULL)
	{
		int n = best.Problem.Size;
		int tabuDuration = U->Calculate(n);  // Parameter 1 (< n^2/2)
		int aspiration = T->Calculate(n);  // Parameter 2 (> n^2/2)
		int runs = iterations != Global::Max ? iterations : Runs->Calculate(n);

		int joltRate = JoltRate->Calculate(n);

		bool authorized;  // move not tabu?
		bool aspired;     // move forced?
		bool alreadyAspired;  // In case many moves forced
		int iBest, jBest;
		double minDelta;
		double r;

		Solution p(best);  // current solution

		double** tabuList = Global::CreateMatrix(n);  // Tabu status
		for (int i = 0; i < n; ++i) 
			for (int j = 0; j < n; ++j)
				tabuList[i][j] = -(n*i + j);	
		
		double** delta = Global::CreateMatrix(n);
		p.SwapCostMatrix(delta);
		
		for (int run = 1; run <= runs; ++run)
		{
			// JOLT every JOLTRATE runs
			if (Jolt != NULL && run != 1 && (run-1)%joltRate == 0)
			{
				if (JoltBest)
					p = best;
				r = p.GetFitness();
				Jolt->Enhance(p, globalIteration);
				if (r != p.GetFitness() || JoltBest) // if jolt actually generated a new solution
					p.SwapCostMatrix(delta); // Recalculate entire swapmatrix because a swap[s] just occurred from jolting.
			}
						
			// Find best move (iBest, jBest) 
			iBest = Global::Max; // in case all moves are tabu 
			jBest = Global::Max;
			minDelta = Global::Max;
			alreadyAspired = false;

			for (int i = 0; i < n-1; ++i) 
				for (int j = i+1; j < n; ++j)
				{
					authorized = tabuList[i][p[j]] < run || tabuList[j][p[i]] < run;
					aspired = tabuList[i][p[j]] < run - aspiration || tabuList[j][p[i]] < run - aspiration || p.GetFitness() + delta[i][j] < best.GetFitness();                
					
					if (aspired && !alreadyAspired || aspired && alreadyAspired && delta[i][j] < minDelta || 
						!aspired && !alreadyAspired && delta[i][j] < minDelta && authorized)
					{
						iBest = i; jBest = j; minDelta = delta[i][j];
						if (aspired) 
							alreadyAspired = true;
					}
				}

			if (iBest == Global::Max) 
				continue; //  All moves are tabu this iteration!
			
			// Swap elements in pos. iBest and jBest
			p.Swap(iBest, jBest, &minDelta);
			
			// Forbid reverse move for a non-uniform random number of iterations (taillard update 3/13/2006)
			r = Global::Rand();
			tabuList[iBest][p[jBest]] = run + (int)(r*r*r*tabuDuration);
			r = Global::Rand();
			tabuList[jBest][p[iBest]] = run + (int)(r*r*r*tabuDuration);
			
			// Best solution improved ?
			if (p.GetFitness() < best.GetFitness())
			{
				//cout << p.GetDeviation() << endl;
				best = p;
				if (ResetIfImprove)
					run = 0;
			}

			// Update matrix of move costs
			if (run+1 <= runs)
				p.UpdateSwapCostMatrix(delta);


			if (runner != NULL)
			{
				runner->Update(best.GetFitness());
				if (runner->IsDone())
					break;
			}
		}

		Global::DeleteMatrix(delta, n);
		Global::DeleteMatrix(tabuList, n);

	}
};      


class Concat : public LocalSearch
{
public:
	LocalSearch *LS1, *LS2;
	
	Concat(LocalSearch *ls1, LocalSearch *ls2) : LS1(ls1), LS2(ls2) {}
	~Concat() { delete LS1; delete LS2; }
	
	inline void Enhance(Solution& solution, int globalIteration,  int iterations = Global::Max, Runner* runner = NULL)
	{
		LS1->Enhance(solution, globalIteration, iterations);
		LS2->Enhance(solution, globalIteration, iterations);
	}
	string ParmsToString()
	{
		stringstream s;
		s << LS1->ToString() << " -> " << LS2->ToString();
		return s.str();
	}
};

class Lin : public LocalSearch
{
public:
	int Arms;
	LocalSearch* Jolt;
	Ratio* Length;  // Length <= n

	Lin(int arms, Ratio* length, LocalSearch* jolt=NULL) : Arms(arms), Jolt(jolt), Length(length) 
	{
		length->x[1] = min(length->x[1],1.0);
		length->x[2] = 0;
	}
	~Lin() { delete Length;  delete Jolt; }

	inline void Enhance(Solution& solution, int globalIteration,  int iterations = Global::Max, Runner* runner = NULL)
	{
		int n = solution.Size();
		int length = min(Length->Calculate(n),n); 
		Permutation A(n);
		Solution tally(solution.Problem);
		int minIndex, bestMinIndex;
		double minCost, bestCost, cost, fitness; 
		double *costs = new double[length], *bestCosts = new double[length];
		int *swapA = new int[length], *swapB = new int[n];
		int *bestSwapA = new int[length], *bestSwapB = new int[length];
		int temp;

		for (int i=0; i<n; ++i)
			swapB[i] = i;

		int iteration = 1;
		do
		{
			bestCost = 0;
			for (int counter=0; counter<Arms; ++counter)
			{
				A.Randomize();
				tally = solution;
				for (int i=0; i<length; ++i)
				{
					minCost=Global::Max;
					swapA[i] = A[i];
					for (int j=i; j<n; ++j)
					{
						cost = tally.SwapCost(swapA[i], swapB[j]);
						if (cost < minCost)
						{
							minCost = cost;
							minIndex = j;
						}
					}
					temp = swapB[i];
					swapB[i] = swapB[minIndex];
					swapB[minIndex] = temp;
					costs[i] = minCost;
					tally.Swap(swapA[i], swapB[i], &minCost); 
				}


				//// UNCOMMENT FOR VARIABLE LIN KERNIGHAN
				//for (int i=0; i<length;)
				//{
				//	A.Randomize();
				//	for (int j=0; j<n&&i<length; ++j,++i)
				//		swapA[i] = A[j];
				//}
				//tally = solution;
				//for (int i=0; i<length; ++i)
				//{
				//	minCost=Global::Max;
				//	for (int j=0; j<n; ++j)
				//	{
				//		if (swapA[i] == j) continue; // Don't swap an item with itself. (this gives cost of 0, and prevents worse solutions from occuring which we need sometimes!)
				//		cost = tally.SwapCost(swapA[i], j);
				//		if (cost < minCost)
				//		{
				//			minCost = cost;
				//			minIndex = j;
				//		}
				//	}
				//	swapB[i] = minIndex;
				//	costs[i] = minCost;
				//	tally.Swap(swapA[i], swapB[i], &minCost);
				//}

				// Find the best running cost.
				minIndex = Global::Max;
				minCost = Global::Max;
				cost = 0;
				for (int i=0; i<length; ++i)
				{
					cost += costs[i];
					if (cost < minCost)
					{
						minIndex = i;
						minCost = cost;
					}
				}

				if (minCost < 0 && minCost < bestCost)
				{
					bestCost = minCost;
					bestMinIndex = minIndex;
					for (int i=0; i<=bestMinIndex; ++i)
					{
						bestCosts[i] = costs[i];
						bestSwapA[i] = swapA[i];
						bestSwapB[i] = swapB[i];
					}
				}
			}
			if (bestCost < 0)
			{
				for (int i=0; i<= bestMinIndex; ++i)
					solution.Swap(bestSwapA[i], bestSwapB[i], &bestCosts[i]);
			}
			else if (Jolt != NULL)  // No arms found any improvement... try to jolt out of local optimum by running one iteration of the jolt local search.
			{	
				fitness = solution.GetFitness();
				Jolt->Enhance(solution, globalIteration, 1);
				bestCost = solution.GetFitness() - fitness;
			}


			++iteration;
		}
		while (bestCost < 0 && iteration <= iterations);
		
		delete[] costs;
		delete[] bestCosts;
		delete [] swapA;
		delete [] swapB;
		delete [] bestSwapA;
		delete [] bestSwapB;
	}

	string ParmsToString()
	{
		stringstream s;
		s << "arms=" << Arms << " length=" << Length->ToString();
		if (Jolt != NULL)
			s << " jolt=" << Jolt->ToString();
		return s.str();
	}

};


class Peek : public LocalSearch
{
private:
	double** costs;
	int rows;	
	
	Ratio* Width;
public:
	Peek(Ratio* width) : Width(width) {}
	~Peek() { delete Width; }

	inline void Enhance(Solution& best, int globalIteration,  int iterations = Global::Max, Runner* runner = NULL)
	{
		double sumBest, sum;
		int iBest, jBest, i, j, x, y;
		bool notDone, localOpt;
		int n = best.Size(), n2 = n*n;
		int width = min(n2/2, Width->Calculate(n));
		double **delta = Global::CreateMatrix(n), **delta2 = Global::CreateMatrix(n);
		best.SwapCostMatrix(delta);
		int *v = new int[n2];
		for (int i=0; i<n2; ++i)
			v[i] = i;
		
		Solution p(best);

		Global::FindMin(delta, n, true, i,j);
		notDone = delta[i][j] < 0;
		while (notDone)
		{
			QuickSort(v, 0, n2-1, delta, n);
			sumBest = Global::Max;
			for (int k=0; k<width; ++k)
			{
				i = v[2*k]/n;  // array is symmetrical so the sorted output is like x,x, y,y, z,z, ..., so skip every other one.
				j = v[2*k]%n;
				p = best;
				p.Swap(i,j, &delta[i][j]);
				Global::CopyMatrix(delta2, delta, n);
				p.UpdateSwapCostMatrix(delta2);
				Global::FindMin(delta2, n, true, x, y);
				sum = delta[i][j] + min(0.0,delta2[x][y]); // The swap at i,j could produce a solution at a local optimum (which means all possible next swaps will be >0)
				if (sum < sumBest)
				{
					localOpt = delta2[x][y] > 0; // it means we're at local opt since min(delta2) > 0
					iBest = i;
					jBest = j;
					sumBest = sum;
				}
			}
			if (sumBest < 0)	
			{
				best.Swap(iBest, jBest, &delta[iBest][jBest]);
				if (!localOpt)
					best.UpdateSwapCostMatrix(delta);
			}
			notDone = !localOpt;
		}
		delete [] v;
		Global::DeleteMatrix(delta, n);
		Global::DeleteMatrix(delta2, n);
	}

	inline void QuickSort(int arr[], int left, int right, double** delta, int rows) 
	{
		  int i = left, j = right;
		  int tmp;
		  int pivot = arr[(left + right) / 2];
 
		  /* partition */
		  while (i < j) {
				while (Compare("<", arr[i], pivot, delta, rows))
					  i++;
				while (Compare(">", arr[j], pivot, delta, rows))
					  j--;
				if (i <= j) {
					  tmp = arr[i];
					  arr[i] = arr[j];
					  arr[j] = tmp;
					  i++;
					  j--;
				}
		  };
 
		  /* recursion */
		  if (left < j)
				QuickSort(arr, left, j, delta, rows);
		  if (i < right)
				QuickSort(arr, i, right, delta, rows);
	}

	inline bool Compare(string op, int i, int j, double** costs, int rows)
	{
		if (op == "<")
			return costs[i/rows][i%rows] < costs[j/rows][j%rows];
		else if (op == ">")
			return costs[i/rows][i%rows] > costs[j/rows][j%rows];
		assert(false);
	}

	string ParmsToString()
	{
		stringstream s;
		s << "width=" << Width->ToString();
		return s.str();
	}

};


class LinVary : public LocalSearch
{
public:
	int Arms;
	LocalSearch* Jolt;
	Ratio *MinLength, *MaxLength;  // Length <= n

	LinVary(int arms, Ratio* minLength, Ratio* maxLength, LocalSearch* jolt=NULL) : Arms(arms), Jolt(jolt), MinLength(minLength), MaxLength(maxLength) 
	{
		minLength->x[1] = min(minLength->x[1],1.0); minLength->x[2] = 0;
		maxLength->x[1] = min(maxLength->x[1],1.0); maxLength->x[2] = 0;
	}
	~LinVary() { delete MaxLength; delete MinLength;  delete Jolt; }

	inline void Enhance(Solution& solution, int globalIteration, int iterations = Global::Max, Runner* runner = NULL)
	{
		int n = solution.Size();
		int maxLength = Global::Constrain(MinLength->Calculate(n), 0, n);
		int minLength = Global::Constrain(MinLength->Calculate(n),0, maxLength);
		Permutation A(n);
		Solution tally(solution.Problem);
		int minIndex, bestMinIndex;
		double minCost, bestCost, cost, fitness; 
		double *costs = new double[maxLength], *bestCosts = new double[maxLength];
		int *swapA = new int[maxLength], *swapB = new int[n];
		int *bestSwapA = new int[maxLength], *bestSwapB = new int[maxLength];
		int temp;

		for (int i=0; i<n; ++i)
			swapB[i] = i;

		int iteration = 1;
		int length = minLength;
		do
		{
			bestCost = 0;
			for (int counter=0; counter<Arms; ++counter)
			{
				A.Randomize();
				tally = solution;
				for (int i=0; i<length; ++i)
				{
					minCost=Global::Max;
					swapA[i] = A[i];
					for (int j=i; j<n; ++j)
					{
						cost = tally.SwapCost(swapA[i], swapB[j]);
						if (cost < minCost)
						{
							minCost = cost;
							minIndex = j;
						}
					}
					temp = swapB[i];
					swapB[i] = swapB[minIndex];
					swapB[minIndex] = temp;
					costs[i] = minCost;
					tally.Swap(swapA[i], swapB[i], &minCost); 
				}

				// Find the best running cost.
				minIndex = Global::Max;
				minCost = Global::Max;
				cost = 0;
				for (int i=0; i<length; ++i)
				{
					cost += costs[i];
					if (cost < minCost)
					{
						minIndex = i;
						minCost = cost;
					}
				}

				if (minCost < 0 && minCost < bestCost)
				{
					bestCost = minCost;
					bestMinIndex = minIndex;
					for (int i=0; i<=bestMinIndex; ++i)
					{
						bestCosts[i] = costs[i];
						bestSwapA[i] = swapA[i];
						bestSwapB[i] = swapB[i];
					}
				}
			}
			if (bestCost < 0)
			{
				for (int i=0; i<= bestMinIndex; ++i)
					solution.Swap(bestSwapA[i], bestSwapB[i], &bestCosts[i]);
			}
			else if (Jolt != NULL)  // No arms found any improvement... try to jolt out of local optimum by running one iteration of the jolt local search.
			{	
				fitness = solution.GetFitness();
				Jolt->Enhance(solution, globalIteration, 1);
				bestCost = solution.GetFitness() - fitness;
			}


			++iteration;
			++length;
			if (length > maxLength) 
				length = minLength;
		}
		while (bestCost < 0 && iteration <= iterations);
		
		delete[] costs;
		delete[] bestCosts;
		delete [] swapA;
		delete [] swapB;
		delete [] bestSwapA;
		delete [] bestSwapB;
	}

	string ParmsToString()
	{
		stringstream s;
		s << "arms=" << Arms << " min=" << MinLength->ToString() << " max=" << MaxLength->ToString();
		if (Jolt != NULL)
			s << " jolt=" << Jolt->ToString();
		return s.str();
	}

};


class LinGlobalVary : public LocalSearch
{
public:
	int Arms;
	LocalSearch* Jolt;
	Ratio *MinLength, *MaxLength;  // Length <= n
	Ratio *GlobalIterationsToMax;

	LinGlobalVary(int arms, Ratio* minLength, Ratio* maxLength, Ratio* globalIterationsToMax, LocalSearch* jolt=NULL) : GlobalIterationsToMax(globalIterationsToMax), Arms(arms), Jolt(jolt), MinLength(minLength), MaxLength(maxLength) 
	{
		minLength->x[1] = min(minLength->x[1],1.0); minLength->x[2] = 0;
		maxLength->x[1] = min(maxLength->x[1],1.0); maxLength->x[2] = 0;
	}
	~LinGlobalVary() { delete GlobalIterationsToMax; delete MaxLength; delete MinLength;  delete Jolt; }

	inline void Enhance(Solution& solution, int globalIteration, int iterations = Global::Max, Runner* runner = NULL)
	{
		int n = solution.Size();
		int maxLength = Global::Constrain(MinLength->Calculate(n), 0, n);
		int minLength = Global::Constrain(MinLength->Calculate(n),0, maxLength);
		Permutation A(n);
		Solution tally(solution.Problem);
		int minIndex, bestMinIndex;
		double minCost, bestCost, cost, fitness; 
		double *costs = new double[maxLength], *bestCosts = new double[maxLength];
		int *swapA = new int[maxLength], *swapB = new int[n];
		int *bestSwapA = new int[maxLength], *bestSwapB = new int[maxLength];
		int temp;

		for (int i=0; i<n; ++i)
			swapB[i] = i;

		int iteration = 1;
		
		int length = minLength + int(double(MaxLength-MinLength)/double(GlobalIterationsToMax->Calculate(n)) * globalIteration + .5);
		do
		{
			bestCost = 0;
			for (int counter=0; counter<Arms; ++counter)
			{
				A.Randomize();
				tally = solution;
				for (int i=0; i<length; ++i)
				{
					minCost=Global::Max;
					swapA[i] = A[i];
					for (int j=i; j<n; ++j)
					{
						cost = tally.SwapCost(swapA[i], swapB[j]);
						if (cost < minCost)
						{
							minCost = cost;
							minIndex = j;
						}
					}
					temp = swapB[i];
					swapB[i] = swapB[minIndex];
					swapB[minIndex] = temp;
					costs[i] = minCost;
					tally.Swap(swapA[i], swapB[i], &minCost); 
				}

				// Find the best running cost.
				minIndex = Global::Max;
				minCost = Global::Max;
				cost = 0;
				for (int i=0; i<length; ++i)
				{
					cost += costs[i];
					if (cost < minCost)
					{
						minIndex = i;
						minCost = cost;
					}
				}

				if (minCost < 0 && minCost < bestCost)
				{
					bestCost = minCost;
					bestMinIndex = minIndex;
					for (int i=0; i<=bestMinIndex; ++i)
					{
						bestCosts[i] = costs[i];
						bestSwapA[i] = swapA[i];
						bestSwapB[i] = swapB[i];
					}
				}
			}
			if (bestCost < 0)
			{
				for (int i=0; i<= bestMinIndex; ++i)
					solution.Swap(bestSwapA[i], bestSwapB[i], &bestCosts[i]);
			}
			else if (Jolt != NULL)  // No arms found any improvement... try to jolt out of local optimum by running one iteration of the jolt local search.
			{	
				fitness = solution.GetFitness();
				Jolt->Enhance(solution, globalIteration, 1);
				bestCost = solution.GetFitness() - fitness;
			}


			++iteration;
		}
		while (bestCost < 0 && iteration <= iterations);
		
		delete[] costs;
		delete[] bestCosts;
		delete [] swapA;
		delete [] swapB;
		delete [] bestSwapA;
		delete [] bestSwapB;
	}

	string ParmsToString()
	{
		stringstream s;
		s << "arms=" << Arms << " min=" << MinLength->ToString() << " max=" << MaxLength->ToString() << " toMax=" << GlobalIterationsToMax->ToString();
		if (Jolt != NULL)
			s << " jolt=" << Jolt->ToString();
		return s.str();
	}

};


class TabuLin : public LocalSearch
{
public:
	int Arms;
	Ratio *U, *Swaps;

	TabuLin(Ratio* swaps, Ratio* u, int arms) : Arms(arms), U(u), Swaps(swaps)  {}
	~TabuLin() { delete U; delete Swaps; }

	inline void Enhance(Solution& best, int globalIteration, int iterations = Global::Max, Runner* runner = NULL)
	{
		int n = best.Size(), minIndex, bestMinIndex, minForbidIndex, temp, iteration, swaps, a, b;
		int maxSwaps = Swaps->Calculate(n);
		double minCost, bestCost, minForbidCost, cost, r, *costs = new double[n], *bestCosts = new double[n];
		int *swapA = new int[n], *swapB = new int[n], *bestSwapA = new int[n], *bestSwapB = new int[n];
		int tabuDuration = U->Calculate(n);
		Permutation A(n);
		Solution p = best, tally(p.Problem);
		bool authorized;
		
		double** tabuList = Global::CreateMatrix(n);  // Tabu status
		for (int i = 0; i < n; ++i) 
			for (int j = 0; j < n; ++j)
				tabuList[i][j] = -(n*i + j);	
		
		for (int i=0; i<n; ++i)
			swapB[i] = i;

		iteration = swaps = 1;
		do
		{
			bestCost = Global::Max;
			for (int counter=0; counter<Arms; ++counter)
			{
				A.Randomize();
				tally = p;
				for (int i=0; i<n; ++i)
				{
					minCost=minForbidCost=Global::Max;
					swapA[i] = A[i];
					for (int j=i; j<n; ++j)
					{
						a = swapA[i]; b = swapB[j];
						cost = tally.SwapCost(a, b);
						authorized = tabuList[a][tally[b]] < swaps + i || tabuList[b][tally[a]] < swaps + i ||
									 tally.GetFitness() + cost < best.GetFitness(); // authorized if not tabu or gives better best solution.
						if (cost < minForbidCost)
						{
							minForbidCost = cost;
							minForbidIndex = j;
						}
						if (authorized && cost < minCost)
						{
							minCost = cost;
							minIndex = j;
						}
					}
					if (minCost == Global::Max) // Everything is forbidden so pick best forbidden one. (We could just stop swapping at this point...) 
					{ 
						minCost = minForbidCost;
						minIndex = minForbidIndex;
					}

					temp = swapB[i];
					swapB[i] = swapB[minIndex];
					swapB[minIndex] = temp;
					costs[i] = minCost;
					tally.Swap(swapA[i], swapB[i], &minCost); 
				}

				// Find the best running cost.
				minIndex = Global::Max;
				minCost = Global::Max;
				cost = 0;
				for (int i=0; i<n; ++i)
				{
					cost += costs[i];
					if (cost < minCost)
					{
						minIndex = i;
						minCost = cost;
					}
				}

				if (minCost < bestCost)
				{
					bestCost = minCost;
					bestMinIndex = minIndex;
					for (int i=0; i<=bestMinIndex; ++i)
					{
						bestCosts[i] = costs[i];
						bestSwapA[i] = swapA[i];
						bestSwapB[i] = swapB[i];
					}
				}
			}
			
			for (int i=0; i<= bestMinIndex; ++i)
			{
				a = bestSwapA[i]; b = bestSwapB[i];
				p.Swap(a, b, &bestCosts[i]);
				// Forbid reverse move for a non-uniform random number of iterations (taillard update 3/13/2006)
				r = Global::Rand();
				tabuList[a][p[b]] = swaps + (int)(r*r*r*tabuDuration);
				r = Global::Rand();
				tabuList[b][p[a]] = swaps + (int)(r*r*r*tabuDuration);
				++swaps;
			}

			if (p.GetFitness() < best.GetFitness())
				best = p;

			++iteration;
		}
		while (iteration <= iterations && swaps <= maxSwaps);
		
		delete[] costs;
		delete[] bestCosts;
		delete [] swapA;
		delete [] swapB;
		delete [] bestSwapA;
		delete [] bestSwapB;
		Global::DeleteMatrix(tabuList,n);
	}

	string ParmsToString()
	{
		stringstream s;
		//s << "arms=" << Arms << " length=" << Length->ToString();
		s << "swaps=" << Swaps->ToString() << " u=" << U->ToString() << " arms=" << Arms ;
		return s.str();
	}

};


class FirstRow : public LocalSearch
{
public:
	int Arms;
	Ratio *Length;
	LocalSearch* Jolt;

	FirstRow(int arms, Ratio *length, LocalSearch *ls=NULL) : Jolt(ls), Arms(arms), Length(length) {} 
	~FirstRow() { delete Jolt; delete Length; }
	
	inline void Enhance(Solution& best, int globalIteration, int iterations=Global::Max, Runner* runner = NULL)
	{
		int n = best.Size();
		int length = Length->Calculate(n);
		best.GetFitness(); // force fitness calculation if needed
		double fitness, minCost, cost;
		int minIndex;
		Solution p(best.Problem), tally(best.Problem);
		Permutation A(n);

		bool bestUpdated;
		int iteration = 1;
		do
		{
			bestUpdated = false;
			p = best;
			for (int arm=0; arm<Arms; ++arm)
			{
				tally = p;
				
				for (int l=0; l<length;)
				{
					A.Randomize(); // Concatenate random sequences of length 'n' to reach 'length'
					for (int i=0; i<n && l<length; ++i,++l)
					{	
						minCost = Global::Max;
						for (int j=0; j<n; ++j)
						{
							cost = tally.SwapCost(A[i],j);  
							if (cost < minCost) // Remember you can swap with yourself so minCost <= 0 always.
							{
								minCost = cost; 
								minIndex = j;
							}
						}

						tally.Swap(A[i], minIndex, &minCost);

					}
				}

				if (tally.GetFitness() < best.GetFitness())
				{
					best = tally;
					bestUpdated = true;
				}
			}
			if (!bestUpdated && Jolt != NULL)  // No arms found any improvement... try to jolt out of local optimum by running one iteration of the jolt local search.
			{	
				fitness = best.GetFitness();
				Jolt->Enhance(best, globalIteration, 1);
				bestUpdated = best.GetFitness() < fitness - 1e-6;
			}

			++iteration;
		} while (bestUpdated && iteration <= iterations);

	}

	string ParmsToString() 
	{
		stringstream s;
		s << "arms=" << Arms << " length=" << Length->ToString();
		if (Jolt != NULL)
			s << " jolt=" << Jolt->ToString();
		return s.str();
	}
	
};

class TabuFirstRow : public LocalSearch
{
public:
	int Arms;
	Ratio *Length, *U, *Swaps;

	TabuFirstRow(Ratio* swaps, Ratio* u, int arms, Ratio *length) : Swaps(swaps), Arms(arms), U(u), Length(length) {} 
	~TabuFirstRow() { delete Length; delete U; delete Swaps;}
	
	inline void Enhance(Solution& best, int globalIteration, int iterations=Global::Max, Runner* runner = NULL)
	{
		int n = best.Size();
		int length = Length->Calculate(n);
		int maxSwaps = Swaps->Calculate(n); 
		double minCost, cost, bestTallyCost, r;
		int minIndex, u = U->Calculate(n), a, b;
		best.GetFitness(); // force fitness calculation if needed
		Solution p = best, tally(best.Problem);
		Permutation A(n);
		

		double** tabuList = Global::CreateMatrix(n,0);
		int *swapA = new int[length], *swapB = new int[length], *bestSwapA = new int[length], *bestSwapB = new int[length];
		double *costs = new double[length], *bestCosts = new double[length];

		bool bestUpdated, authorized;
		int iteration = 1, swaps = 1;
		do
		{
			bestUpdated = false;
			for (int arm=0; arm<Arms; ++arm)
			{
				tally = p;
				
				for (int l=0; l<length;)
				{
					A.Randomize(); // Concatenate random sequences of length 'n' to reach 'length'
					for (int i=0; i<n && l<length; ++i,++l)
					{	
						bestTallyCost = Global::Max;
						minCost = Global::Max;
						for (int j=0; j<n; ++j)
						{
							a = A[i]; 
							b = j;

							cost = tally.SwapCost(a,b); 
							
							authorized = tabuList[a][tally[b]] < swaps + i || tabuList[b][tally[a]] < swaps + i ||
										  tally.GetFitness() + cost < best.GetFitness();

							if (cost < minCost && authorized)
							{
								minCost = cost; 
								minIndex = j;
							}
						}
						
						if (minCost == Global::Max) // All moves are tabu
						{
							minCost = 0;
							minIndex = i; // Swap with yourself just to skip this iteration of i.
						}

						swapA[l] = A[i];
						swapB[l] = minIndex;
						costs[l] = minCost;

						tally.Swap(A[i], minIndex, &minCost);

					}
				}

				if (tally.GetFitness() < bestTallyCost)
				{
					bestTallyCost = tally.GetFitness();

					for (int i=0; i<length; ++i)
					{
						bestSwapA[i] = swapA[i];
						bestSwapB[i] = swapB[i];
						bestCosts[i] = costs[i];
					}
				}
			}

			for (int i=0; i<length; ++i)
			{
				a = bestSwapA[i]; b = bestSwapB[i];
				p.Swap(a,b,&bestCosts[i]);
				// Forbid reverse move for a non-uniform random number of iterations (taillard update 3/13/2006)
				r = Global::Rand();
				tabuList[a][p[b]] = swaps + (int)(r*r*r*u);
				r = Global::Rand();
				tabuList[b][p[a]] = swaps + (int)(r*r*r*u);
				++swaps;			
			}

			if (p.GetFitness() < best.GetFitness())
				best = p;
			++iteration;
		} while (iteration <= iterations && swaps <= maxSwaps);

		Global::DeleteMatrix(tabuList,n);
		delete [] costs;
		delete [] bestCosts;
		delete [] swapA;
		delete [] swapB;
		delete [] bestSwapA;
		delete [] bestSwapB;
	}

	string ParmsToString() 
	{
		stringstream s;
		s << "swaps=" << Swaps->ToString() << " u=" << U->ToString() << " arms=" << Arms << " length=" << Length->ToString();
		return s.str();
	}
	
};

// Fast descent procedure [Adaptive Memories for QAP, taillard, 1997]
class FastDescent : public LocalSearch
{
public:
	Ratio* Length;
	FastDescent(Ratio* length=NULL)
	{
		Length = length== NULL ? new Ratio(0,2,0) : length; // default 2n
	}
	~FastDescent() { delete Length; }
	inline void Enhance(Solution& solution, int globalIteration, int iterations=Global::Max, Runner* runner = NULL)
	{
		int n = solution.Size();
		Permutation a(n), b(n);
		int length = Length->Calculate(n);
		bool swapped;
		double cost;

		while (true)
		{
			swapped = false;
			a.Randomize();
			for (int i=0; i<n; ++i)
			{
				b.Randomize();
				for (int j=0; j<n; ++j)
				{
					if ((cost=solution.SwapCost(a[i],b[j])) < 0)
					{	
						solution.Swap(a[i],b[j],&cost);
						swapped = true;
					}
					--length;
					if (length <= 0) break;
				}
			}
			if (!swapped) break; // we considered all neighbors which means we're at a local optima.  
		} 

	}
	string ParmsToString() 
	{
		stringstream s;
		s << " length=" << Length->ToString();
		return s.str();
	}
};

class First : public LocalSearch
{
public:
	int Arms;
	Ratio *Length;
	LocalSearch* Jolt;

	First(int arms, Ratio *length, LocalSearch *ls=NULL) : Jolt(ls), Arms(arms), Length(length) {} 
	~First() { delete Jolt; delete Length; }
	
	inline void Enhance(Solution& best, int globalIteration, int iterations=Global::Max, Runner* runner = NULL)
	{
		int n = best.Size();
		int armLength = Length->Calculate(n);
		best.GetFitness(); // force fitness calculation if need be
		double fitness;
		Solution p(best.Problem), tally(best.Problem);

		bool bestUpdated;
		int iteration = 1;
		do
		{
			bestUpdated = false;
			p = best;
			for (int i=0; i<Arms; ++i)
			{
				tally = p;
				FastDescent(tally, armLength);
				if (tally.GetFitness() < best.GetFitness())
				{
					best = tally;
					bestUpdated = true;
				}
			}
			if (!bestUpdated && Jolt != NULL)  // No arms found any improvement... try to jolt out of local optimum by running one iteration of the jolt local search.
			{	
				fitness = best.GetFitness();
				Jolt->Enhance(best, globalIteration, 1);
				bestUpdated = best.GetFitness() < fitness - 1e-6;
			}
			++iteration;
		} while (bestUpdated && iteration <= iterations);

	}
	
	// Fast descent procedure [Adaptive Memories for QAP, taillard, 1997]
	inline bool FastDescent(Solution& solution, int totalTimes)
	{
		Permutation a(solution.Size()), b(solution.Size());
		int n = solution.Size();
		double cost;
		bool swapped = false;
		int currentTimes = 0;
		while (true)
		{
			a.Randomize();
			for (int i=0; i<n; ++i)
			{
				b.Randomize();
				for (int j=0; j<n; ++j)
				{
					if ((cost=solution.SwapCost(a[i],b[j])) < 0)
					{	
						solution.Swap(a[i],b[j],&cost);
						swapped = true;
					}
					++currentTimes;
					if (currentTimes >= totalTimes)
						return swapped;
				}
			}
		}
	}

	string ParmsToString() 
	{
		stringstream s;
		s << "arms=" << Arms << " length=" << Length->ToString();
		if (Jolt != NULL)
			s << " jolt=" << Jolt->ToString();
		return s.str();
	}
};

class BestTwoExchange : public LocalSearch
{
public:
	inline void Enhance(Solution& solution, int globalIteration, int iterations=Global::Max, Runner* runner = NULL)
	{
		int n = solution.Size();
		double min;
		double **cost = Global::CreateMatrix(n);
		solution.SwapCostMatrix(cost);
		int r,s;
		int iteration = 1;
		do
		{
			min = Global::Max; 
			for (int i=0; i<n; ++i)
				for (int j=i+1; j<n; ++j)
					if (cost[i][j] < min)
					{
						min = cost[i][j];
						r = i;
						s = j;
					}
			if (min < 0)
			{
				solution.Swap(r,s, &min);
				if (iteration+1 <= iterations)
					solution.UpdateSwapCostMatrix(cost);
			}
			++iteration;
		} while (min < 0 && iteration <= iterations);
		Global::DeleteMatrix(cost,n);
	}
};

class ThreeExchange : public LocalSearch
{
public:
	inline void Enhance(Solution& solution, int globalIteration, int iterations = Global::Max, Runner* runner = NULL)
	{
		while (BestImprovement(solution)) {}
	}	

	inline bool BestImprovement(Solution& solution)
	{
		double min = Global::Max, minPart, total, part;
		int r,s,t;
		for (int i=0; i<solution.Size(); ++i)
			for (int j=0; j<solution.Size(); ++j)
			{
				Solution sol(solution); // O(n) to copy
				part = sol.SwapCost(i,j); 
				sol.Swap(i,j, &part);
				for (int k=0; k<solution.Size(); ++k)
				{
					total = part + sol.SwapCost(j,k); // Because i,j,k intersect swapcost will take O(n)!!!! Not good!!
					if (total < min)
					{
						min = total;
						minPart = part;
						r = i;
						s = j;
						t = k;
					}
				}
			}
		if (min < 0)
		{
			solution.Swap(r,s,&minPart); // Put 0 here or else it will internally clear fitness due to a swap.
			double cost = min-minPart;
			solution.Swap(s,t,&cost);
		}
		return min < 0;
	}
};




class MaybeBest : public LocalSearch 
{
public:
	int Cycles, StartPower, EndPower;
	Ratio* FailCount;
	MaybeBest(int cycles, int startPower, int endPower, Ratio* failCount) : Cycles(cycles), StartPower(startPower), EndPower(endPower), FailCount(failCount) {} 
	~MaybeBest() {delete FailCount;}

	inline void Enhance(Solution& best, int globalIteration, int iterations=Global::Max, Runner* runner = NULL)
	{
		int n = best.Size(), power, cycle, fails, failCount = FailCount->Calculate(n), bestI, bestJ;

		Solution p = best;
		double** delta = Global::CreateMatrix(n);
		p.SwapCostMatrix(delta); // populate with costs.
	
		cycle = 0, power = StartPower, fails = 0;
		while (cycle < Cycles)
		{
			Global::InverseRoulette(delta,n,true,best.GetFitness()-p.GetFitness(), power, &bestI, &bestJ);
			p.Swap(bestI, bestJ, &delta[bestI][bestJ]);
			p.UpdateSwapCostMatrix(delta);
			if (p.GetFitness() < best.GetFitness())
			{	
				best = p; 
				fails = 0; 
				//cycle = 0;
			}
			else
				++fails;

			if (fails > failCount)
			{
				fails = 0; 
				++power;
				if (power > EndPower)
				{
					power = StartPower;
					++cycle;
				}
			}
		}

		Global::DeleteMatrix(delta, n);
	}

	string ParmsToString()
	{
		stringstream s;
		s << "cycles=" << Cycles << " power=[" << StartPower << "," << EndPower << "] fails=" << FailCount->ToString();  	
		return s.str();
	}
	
};
