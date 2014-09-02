#pragma once
#include "Statistics.cpp"
#include "Instance.cpp"
#include "Runner.cpp"
#include <sstream>
#include "AlgoParms.cpp"
using namespace std;

class Result
{
public:
	Instance& Problem;
	int ParmsKey;
	Statistics Fitness;
	Statistics Elapsed;
	Statistics Deviation;

	Result(Instance& instance, int parmsKey) : Problem(instance), ParmsKey(parmsKey) {}
	void Add(Runner runner) { Fitness.Add(runner.GetFitness()); Deviation.Add(runner.Deviation()); Elapsed.Add(runner.RunTime()); }
	
	string ToString()
	{
		stringstream s;
		s << setprecision(3);
		s << setw(2) << ParmsKey <<  ":" << setw(4) << int(Elapsed.Mean()+.5)<< "±" << int(Elapsed.StandardDeviation()+.5) << "s " << setw(10) << int(Fitness.Mean()) << " " << setw(5) << Deviation.Mean() << "±" << Deviation.StandardDeviation() << "%";
		return s.str();
	}

};

