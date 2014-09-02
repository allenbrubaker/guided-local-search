#pragma once
#include <time.h>
#include "Global.cpp"
#include "Instance.cpp"

using namespace std;

class Runner
{
private:
	double Optimal;
	double Fitness;
	int Time;
	time_t StartTime, LastUpdateTime;
	int Elapsed() { return (int)difftime(time(NULL),StartTime); }
public:
	Instance& Problem;
	int Runs, Iteration;
	int Iterations;

	Runner(Instance& problem, int runs, int time, int iterations) : Problem(problem), Optimal(problem.OptimalFitness), Runs(runs), Time(time), Iterations(iterations) {}
	
	inline void Run()
	{
		StartTime = time(NULL);
		Fitness = Global::Max;
		Iteration = 0;
	}

	inline void Update(double fitness)
	{
		if (fitness < Fitness && !IsDone()) 
		{
			Fitness = fitness;
			LastUpdateTime = time(NULL);
		}
	}

	inline void Iterate() { ++Iteration; }
	inline double GetFitness() { return Fitness; }

	inline int RunTime() { return (int)difftime(LastUpdateTime,StartTime); } 
	inline double Deviation() { return (Fitness-Optimal)/Optimal * 100; } 
	inline bool IsDone() { return Elapsed() > Time || Deviation() < 1e-6 || Iteration >= Iterations;}
	//void QuitIfDone() { if (IsDone()) throw this; }
	//void QuitOrUpdate(double fitness) { QuitIfDone(); Update(fitness); }

	inline bool IsLastIteration() { return Iteration == Iterations-1; }

};