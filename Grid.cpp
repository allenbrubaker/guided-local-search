#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <string>

#include "Algorithm.cpp"
#include "Global.cpp"
#include "Result.cpp"
#include "Instance.cpp"
#include "Statistics.cpp"
using namespace std;


void OpenFile(string);

class Grid
{
private:
	vector<Algorithm*>& Algos;
	fstream Log;
	string Description;
	int Runtime, Runs, Iterations;
	int Cols;
	Instance* CurrentProblem;
	Statistics* TotalDeviation;
	Statistics* TotalBestDeviation;
	Statistics* TotalBestCount;
public:
	Grid(string fileName, string description, int runs, int runTime, int iterations, vector<Algorithm*>& algos) : Algos(algos), CurrentProblem(NULL), Cols(0), Runs(runs), Runtime(runTime), Iterations(iterations), Description(description) 
	{
		Global::OpenFile(fileName, Log);
		assert(algos.size() > 0);
		TotalDeviation = new Statistics[algos.size()];
		TotalBestDeviation = new Statistics[algos.size()];
		TotalBestCount = new Statistics[algos.size()];
	}

	void PrintHeader()
	{
		Log << Description << endl << endl;
		Log << "RUNS = " << Runs << "   RUNTIME = " << Runtime << "s   ITERATIONS = " << Iterations <<  endl;
		Log << endl;
		Log << "PARAMETERS" << endl;
		for (int i=0; i<Algos.size(); ++i)
			Log << Algos[i]->GetParms()->ToString() << endl;
		Log << endl;

		Log << ",,,, ";
		for (int i=0; i<Algos.size(); ++i)
			Log << "[" << Algos[i]->GetParms()->Key << "]" << (i==Algos.size()-1?"":",,, ");
		Log << endl;
		Log.flush();
	}

	inline int Round(double x) { return int(x+.5); }

	void PrintRowHeader(const Instance& Problem) 
	{
		Log << Problem.Type << ", " << Problem.InstanceName << ", " << Problem.OptimalAlgorithm << ", ";
		Log.flush();
	}

	void Print(Result& result)
	{
		if (Cols == 0 || Cols == Algos.size())
		{
			CurrentProblem = &result.Problem;
			PrintRowHeader(*CurrentProblem);
			Cols = 1;
		}
		else ++Cols;
		
		assert(CurrentProblem == &result.Problem); // Make sure result aligns with current instance.
		assert(result.ParmsKey == Algos[Cols-1]->GetParms()->Key); // Make sure it aligns with the current column algorithm.
		
		TotalDeviation[Cols-1].Add(result.Deviation.Mean());
		TotalBestDeviation[Cols-1].Add(result.Deviation.Min);
		TotalBestCount[Cols-1].Add(result.Deviation.MinCount);

		Log << result.Deviation.Mean() << " ± " << result.Deviation.StandardDeviation() << ", " << result.Deviation.Min << "(" << result.Deviation.MinCount << "), " <<  Round(result.Elapsed.Mean()) << "±" << Round(result.Elapsed.StandardDeviation()) << "s, ";
		if (Cols == Algos.size()) 
			Log << endl;
		Log.flush();
	}

	void NewLine(int count=1) { for (int i=0;i<count; ++i) Log << endl; Log.flush(); }
	void PrintFooter()
	{
		Log << ",, Totals, ";
		for (int i=0; i<Algos.size(); ++i)
			Log << TotalDeviation[i].Total << ", " << TotalBestDeviation[i].Total << "(" << Round(TotalBestCount[i].Total) << "),, ";
		Log << endl << endl << endl;

		Log << Instance::TypesDescription();
		Log.flush();
	}

	~Grid() 
	{ 
		Log.close(); 
		delete [] TotalDeviation;
		delete [] TotalBestDeviation;
		delete [] TotalBestCount;
	}
	
};
