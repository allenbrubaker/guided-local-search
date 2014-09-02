#pragma once

using namespace std;
#include <vector>
#include "Ratio.cpp"
#include "Global.cpp"
#include "Algorithm.cpp"
#include "Instance.cpp"
#include <assert.h>

class Parameter
{
private:
	Ratio *LowerBound, *UpperBound;
	bool IsDiscrete;
	double Value;
public:
	string Name;

	Parameter(string name, Ratio* lowerBound, Ratio* upperBound, bool isDiscrete) : Name(name), LowerBound(lowerBound), UpperBound(upperBound), IsDiscrete(isDiscrete)
	{
	}

	inline double Calculate(double value, int n)
	{
		double p1 = LowerBound->ToDouble(n);
		double p2 = UpperBound->ToDouble(n);
		double x = p1 + value * (p2 - p1);
		return IsDiscrete ? int(x+.5) : x;
	}
	~Parameter()
	{
		delete LowerBound;
		delete UpperBound;
	}
};

class ParticleCoordinate
{
	Parameter& Parm;
public:
	double Value; // [0,1]

	ParticleCoordinate(Parameter& parameter) : Parm(parameter) 
	{
		Value = Global::Rand();
	}

	inline double Calculate(int n)
	{
		return Parm.Calculate(Value, n);
	}

	inline string ToString(int n)
	{
		Ratio *r = Ratio::FromValue(Value, n);
		string s = r->ToString();
		delete r;
		return s;
	}
};

class Particle
{
	vector<Parameter*>& Parms;
	vector<ParticleCoordinate*> coordinates;
public:
	Particle(vector<Parameter*>& parms) : Parms(parms)
	{
		for (int i=0; i<parms.size();  ++i)
		{
			coordinates.push_back(new ParticleCoordinate(*parms[i]));
		}
	}
	inline double& operator[](int index) { return coordinates[index]->Value; }
	inline int size() { return Parms.size(); }
	inline ParticleCoordinate& Coordinate(int index) { return *coordinates[index]; }

	// Old parm pointer must be manually deleted
	inline void Apply(Algorithm& algo, int n)
	{
		algo.CreateParms(false); // don't delete
		for (int i=0; i<coordinates.size(); ++i)
		{
			Ratio* r = Ratio::FromValue(coordinates[i]->Value, n);
			algo.GetParms()->Set(Parms[i]->Name, *r);
			delete r;
		}
	}

	~Particle() 
	{
		for (int i=0; i<coordinates.size(); ++i)
			delete coordinates[i];
	}

};

class ParticleSwarmOptimization
{
private:
	vector<Particle*> Particles;
	Algorithm* Algo;
	fstream Log;
	vector<Parameter*>& Parms;
	vector<Instance*>& Instances;
	double **Means, Best;
public:
	ParticleSwarmOptimization(string fileName, Algorithm* algo, vector<Parameter*>& parms, vector<Instance*>& instances) : Algo(algo), Parms(parms), Instances(instances) 
	{
		Global::OpenFile(fileName, Log);
		Means = new double*[2];
		for (int i=0; i<2; ++i)
			Means[i] = new double[instances.size()];
		Best = Global::Max;
	}


	void Run(int swarmSize = 10, int generations=Global::Max, int averageRunsPerInstance=5, int runTimePerInstance=600)
	{
		assert(Parms.size() > 1);
		assert(Instances.size() > 0);

		double delta, rand;
		AlgoParms* algoParms1;
		AlgoParms* algoParms2;

		for (int i=0; i<swarmSize; ++i)
			Particles.push_back(new Particle(Parms));

		for (int gen=0; gen < generations; ++gen)
		{
			for (int _p=0; _p<swarmSize; ++_p)
			{
				int _p2;
				while ((_p2 = Global::Rand(swarmSize)) != _p);
				Particle& p = *Particles[_p];
				Particle& p2 = *Particles[_p2];
				
				Particle* pTest = new Particle(Parms);
				for (int coord=0; coord<Parms.size(); ++coord)
				{
					delta = fabs(p[coord] - p2[coord]);
					// generate random number inclusively between [-delta, delta]
					rand = Global::RandDouble(-delta, delta);
					(*pTest)[coord] = min<double>(max<double>(0, p[coord] + rand), 1);
				}
				
				// Run on P
				double pDeviation=0, pTestDeviation=0;
				Result* result;
				for (int i=0; i<Instances.size(); ++i)
				{
					Runner run(*Instances[i], averageRunsPerInstance, runTimePerInstance, Global::Max); 
					p.Apply(*Algo, Instances[i]->Size); // Apply Particle to Algo parameters. 
					algoParms1 = Algo->GetParms();
					result = Algo->Run(run);
					pDeviation += result->Deviation.Mean();
					Means[0][i] = result->Deviation.Mean();
					delete result;
				}
				// Run on PTest
				for (int i=0; i<Instances.size(); ++i)
				{
					Runner run(*Instances[i], averageRunsPerInstance, runTimePerInstance, Global::Max); 
					p.Apply(*Algo, Instances[i]->Size); // Apply Particle to Algo parameters. 
					algoParms2 = Algo->GetParms();
					result = Algo->Run(run);
					pTestDeviation += result->Deviation.Mean();
					Means[1][i] = result->Deviation.Mean();
					delete result;
				}
				if (pDeviation < Best || pTestDeviation < Best)
				{
					Particle &pBest = pDeviation < pTestDeviation ? p : *pTest;
					AlgoParms *ap = pDeviation < pTestDeviation ? algoParms1 : algoParms2;
					Best = min<double>(pDeviation, pTestDeviation);
					int index = pDeviation < pTestDeviation ? 0 : 1;
					Log << Best << " ("; 
					
					for (int i = 0; i<Instances.size(); ++i)
						Log <<  Instances[i]->InstanceName << ":" << Means[index][i] << " ";
					Log << ") " << ap->ToString() << endl;
					Log.flush();
					delete algoParms1;
					delete algoParms2;
				}

				if (pTestDeviation < pDeviation)
				{
					delete Particles[_p];
					Particles[_p] = pTest;
				}
				else 
				{
					delete pTest;
				}
			}
		}
	}

	~ParticleSwarmOptimization() 
	{
		Log.close();
		for (int i=0; i<Particles.size(); ++i)
			delete Particles[i];
		for (int i=0; i<2; ++i)
			delete [] Means[i];
		delete Means;
		delete Algo;
	}

	
};