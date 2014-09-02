#pragma once

#include <iostream>
#include "LocalSearch.cpp"
#include "Solution.cpp"
#include "Instance.cpp"
#include "Global.cpp"
#include "Statistics.cpp"
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class LocalSearchAnalysis
{
private:
	vector<LocalSearch*>& LocalSearches;
	fstream File;
public:
	LocalSearchAnalysis(vector<LocalSearch*>& searches) : LocalSearches(searches) {}
	
	void Header()
	{
		for (int i=0; i<LocalSearches.size(); ++i)
		{
			File << "[" << i+1 << "]," << LocalSearches[i]->ToString() << endl; 
		}
		File.flush();
	}

	void ColHeader()
	{
		File << endl;
		RowHeader();
		for (int i=0; i<LocalSearches.size(); ++i)
			File << "[" << i+1 << "],,,,";
		File << endl;
		RowHeader();
		for (int i=0; i<LocalSearches.size(); ++i)
			File << "abs, rel, hamming, sign,";
		File << endl;
		File.flush();
	}

	void RowHeader(Instance *p = NULL)
	{
		if (p != NULL)
			File << p->Type << "," << p->InstanceName << "," << p->OptimalAlgorithm << ",";
		else File << ",,,";
		File.flush();
	}

	void Analyze(vector<Instance*>& instances, int runsPerInstance, int runs, string fileName, bool restart, int perturb=3)
	{
		Global::OpenFile(fileName, File);
		
		vector<Solution*> best;
		for (int i=0; i<LocalSearches.size(); ++i)
			best.push_back(NULL);

		Header();
		ColHeader();
		for (int inst=0; inst<instances.size(); ++inst)
		{
			Instance* p = instances[inst];
			
			Solution s(*instances[inst]);
			s.GetFitness();
			for (int i=0; i<LocalSearches.size(); ++i)
			{
				delete best[i];
				best[i] = new Solution(s);
			}

			for (int iter=0; iter<runsPerInstance; ++iter)
			{
				if (restart)
				{	
					s.Randomize();
					s.GetFitness();
				}

				RowHeader(iter == 0 ? p : NULL);
				for (int ls=0; ls<LocalSearches.size(); ++ls)
				{
					if (!restart)
					{
						s = *best[ls];
						for (int i=0; i<perturb; ++i)
						{
							int r1 = Global::Rand(s.Size());
							int r2;
							while ((r2 = Global::Rand(s.Size())) == r1) {}
							double cost = s.SwapCost(r1,r2);
							s.Swap(r1,r2, &cost);
						}
					}
					LocalSearch *l = LocalSearches[ls];
					Statistics relative, absolute, hamming, sign;
					for (int run=0; run<runs; ++run)
					{
						if (run == 0) { delete best[ls]; best[ls] = NULL; }
						Solution ss = s;
						l->Enhance(ss, 0);
						if (best[ls] == NULL || ss.GetFitness() < best[ls]->GetFitness())
							best[ls] = new Solution(ss);
						double abs = ss.GetDeviation();
						double rel = abs - s.GetDeviation();
						absolute.Add(abs);
						relative.Add(rel);
						hamming.Add(ss.HammingDistance(s));
						sign.Add(ss.SignDistance(s));
					}

					File << absolute.Mean() << " ± " << absolute.StandardDeviation() << "," 
						 << relative.Mean() << "," 
						 << hamming.Mean()  << " ± " << hamming.StandardDeviation() << ","
						 << sign.Mean()     << " ± " << sign.StandardDeviation() << ",";
					File.flush();
				}
				File << endl;
			}
		}
		File.close();
	}

};