#pragma once
#include "Instance.cpp"
#include "Global.cpp"
#include "Permutation.cpp"


class Solution : public Permutation
{
private: 
	double *Fitness;
	int LastSwap[2];
public:
	double *LastSwapCost;
	const Instance& Problem;
	Solution(const Instance& instance) : Permutation(instance.Size), Problem(instance), LastSwapCost(NULL), Fitness(NULL)
	{
		LastSwap[0] = -1; LastSwap[1] = -1;
	}

	Solution(const Solution& solution) : Permutation(solution.Problem.Size), Problem(solution.Problem), LastSwapCost(NULL), Fitness(NULL)
	{
		operator=(solution);
	}

	Solution& operator=(const Solution &solution)
	{
		assert(&solution.Problem == &Problem);
		for (int i=0; i<Problem.Size; ++i)
			Values[i] = solution.Values[i];
		ClearFitness();
		Fitness = solution.Fitness != NULL ? new double(*solution.Fitness) : NULL;
		LastSwapCost = solution.LastSwapCost != NULL ? new double(*solution.LastSwapCost) : NULL;
		LastSwap[0] = solution.LastSwap[0]; 
		LastSwap[1] = solution.LastSwap[1];		
		return *this;
	}

	inline double GetDeviation()
	{
		return (GetFitness() - Problem.OptimalFitness)/Problem.OptimalFitness * 100;
	}

	inline double GetFitness()
	{
		if (Fitness != NULL)
			return *Fitness;
		double sum = 0;
		for (int i=0; i<Problem.Size; ++i)
			for (int j=0; j<Problem.Size; ++j)
				sum += Problem.Distance[i][j] * Problem.Flow[Values[i]][Values[j]];
		Fitness = new double(sum);
		return sum;
	}

	inline void Swap(int i, int j, double* swapCost)
	{
		if (i==j) return;
		Permutation::Swap(i,j);
		LastSwap[0] = i; 
		LastSwap[1] = j;
		if (swapCost != NULL)
		{
			delete LastSwapCost;
			LastSwapCost = new double(*swapCost);
			if (Fitness != NULL)
				*Fitness += *swapCost;
		}
		else
			ClearFitness();
	}
	inline int Size() { return Problem.Size; }
	inline int& operator[](int index) { return Values[index]; }

	inline void SwapCostMatrix(double** matrix)
	{
		for (int i=0; i<Size(); ++i)
			for (int j=i; j<Size(); ++j) // set j=i to zero out the diagonal.
				matrix[j][i] = matrix[i][j] = SwapCost(i,j);
	}

	inline void UpdateSwapCostMatrix(double** matrix)
	{
		for (int i=0; i<Size(); ++i)
			for (int j=i+1; j<Size(); ++j)
				matrix[j][i] = matrix[i][j] = FastSwapCost(matrix[i][j], i, j); 
	}

	inline double FastSwapCost(double lastSwapCostUV, int u, int v)
	{
		int r = LastSwap[0], s = LastSwap[1];
		if (r != u && r != v && s != u && s != v)  // Condition: {r,s} intersect {u,v} == NULL
		{
			double **a = Problem.Distance, **b = Problem.Flow;
			int pu = Values[u], pv = Values[v], pr = Values[r], ps = Values[s];
			return lastSwapCostUV + (a[r][u]-a[r][v]+a[s][v]-a[s][u]) * (b[ps][pu]-b[ps][pv]+b[pr][pv]-b[pr][pu]) 
								 + (a[u][r]-a[v][r]+a[v][s]-a[u][s]) * (b[pu][ps]-b[pv][ps]+b[pv][pr]-b[pu][pr]);
		}
		return SwapCost(u,v);
	}

	inline double SwapCost(int r, int s)
	{
		if (r == s) return 0;
		double **a = Problem.Distance, **b = Problem.Flow;
		int pr = Values[r], ps = Values[s];
		
		double sum = a[r][r]*(b[ps][ps]-b[pr][pr]) + a[r][s]*(b[ps][pr]-b[pr][ps]) + 
			         a[s][r]*(b[pr][ps]-b[ps][pr]) + a[s][s]*(b[pr][pr]-b[ps][ps]); 
		for (int k=0; k<Problem.Size; ++k)
		{
			if (k == r || k == s) continue;
			int pk = Values[k];
			sum += a[k][r]*(b[pk][ps]-b[pk][pr]) + a[k][s]*(b[pk][pr]-b[pk][ps]) + 
				   a[r][k]*(b[ps][pk]-b[pr][pk]) + a[s][k]*(b[pr][pk]-b[ps][pk]);
		}
		return sum;
	}
	
	inline void ClearFitness()
	{
		delete LastSwapCost;
		LastSwapCost = NULL;
		delete Fitness;
		Fitness = NULL;
	}
	~Solution()
	{
		ClearFitness();
	}
};
