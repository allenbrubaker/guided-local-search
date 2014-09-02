#pragma once
#include <string>
#include <sstream>
#include "LocalSearch.cpp"
#include "Construction.cpp"
#include "Ratio.cpp"
#include <deque>
#include <assert.h>
using namespace std;


class AlgoParms
{
private: 
	static int count;
public:
	int Key;
	bool Debug;
	AlgoParms()
	{ 
		Key = -1;
		Debug = false;
	}

	inline string ToString()
	{
		if (Key == -1)
			Key = count++;
		stringstream s;
		s << setprecision(2) << Key << ":  [" << Name() << "] ";
		Append(s);
		return s.str();
	}
	virtual inline string Name() = 0;
	virtual inline void Append(stringstream&) = 0;
	virtual inline void Calculate(int n) {}
	virtual inline void Set(string property, Ratio& r) { assert(false); }
};

class RandomWalkParms : public AlgoParms
{
public:
	LocalSearch* Search;
	RandomWalkParms()
	{
		Search = NULL;
	}

	inline string Name() { return "Walk"; }
	inline void Append(stringstream& s)
	{
		s << "LS=" << (Search==NULL ? "." : Search->ToString());
	}

};

class SteepGLSParms : public AlgoParms
{
public:

	// Base (required)
	Ratio* PerturbMin;
	int perturbMin;
	Ratio* PerturbMax;
	int perturbMax;
	int perturb;
	
	inline bool IsTabuSearch() { return TabuLength != NULL && TabuSearchSwaps != NULL; }
		Ratio* TabuLength;
		int tabuLength;
		Ratio* TabuSearchSwaps;
		int tabuSearchSwaps;

	bool IsMutateByDistance;

	SteepGLSParms() : PerturbMin(NULL), PerturbMax(NULL), TabuLength(NULL), TabuSearchSwaps(NULL), IsMutateByDistance(false)
	{}

	inline void Calculate(int n)
	{
		assert(PerturbMax != NULL && PerturbMin != NULL);
		perturbMin = Global::Constrain(PerturbMin->Calculate(n), 1, n);
		perturbMax = Global::Constrain(PerturbMax->Calculate(n), perturbMin, n);
		perturb = perturbMin;

		if (IsTabuSearch()) { tabuLength = TabuLength->Calculate(n); tabuSearchSwaps = TabuSearchSwaps->Calculate(n); }
	}
	inline void IncreasePerturb()
	{
		++perturb;
		if (perturb > perturbMax) 
			ResetPerturb();
	}
	inline void ResetPerturb() { perturb = perturbMin; }

	inline string Name() { return "S-GLS"; }
	inline void Append(stringstream& s)
	{
		s << " p=" << PerturbMin->ToString() << ".." << PerturbMax->ToString();
		if (IsTabuSearch()) s << " ts=" << TabuLength->ToString() << ":" << TabuSearchSwaps->ToString(); 
		if (IsMutateByDistance) s << " byDist";
	}

	~SteepGLSParms() 
	{
		delete PerturbMin; delete PerturbMax;
		delete TabuLength; delete TabuSearchSwaps;
	}
};

enum kEvaporateMode { EvapNone, EvapByPenalty, EvapAllByPenalty, EvapByLate, EvapAllByLate };
enum kPenaltyMode { IncreaseByUtility, IncreaseAllByUtility, IncreaseByCost, IncreaseAllByCost };
class GLSParms : public AlgoParms
{
public:
	// Base (required)
	
	double Lambda; // 1.0         //  lambda coefficient  
	bool IsAspireBest; // true   //  aspiration criterion 1 - always defer to best swap disregarding augmented objective function if it produces best known value.  
	kPenaltyMode penaltyMode; // IncreaseByUtility
	double penaltyAmount;  // 1.0

	// Extensions (optional)
	
	inline bool IsAspireGood() { return AspireGood != NULL; } 
		Ratio* AspireGood;
		int aspireGood;

	inline bool IsAspireLate() { return AspireLate != NULL; } // aspiration criterion 2 - force a swap if it hasn't been seen for a while.
		Ratio* AspireLate; 
		int aspireLate;

	inline bool IsRandomMove() { return RandomMovePr > 0.0 && RandomMovePr <= 1.0; }  
		double RandomMovePr; // (0,1] // probability of moving to a random neighbor while locally searching augmented function

	inline bool IsBestMoveInterval() { return BestMoveInterval != NULL; } 
		Ratio* BestMoveInterval; // rate to completely ignore augmented function and use original objective function
		int bestMoveInterval; 
	
	inline bool IsBestMovePr() { return BestMovePr > 0.0 && BestMovePr <= 1.0; }
		double BestMovePr; // (0,1] // Same as BestMoveInterval but with a probability of bestMove every iteration instead.


	inline bool IsPenaltyNoise() { return PenaltyNoisePr > 0.0 && PenaltyNoisePr <= 1.0; } // maximum amount that penalties of two locations in a neighboring swap will be reduced and original objective function will be used
		double PenaltyNoisePr; // (0,1]
	
	inline bool IsSteepestDescentInterval() { return SteepestDescentInterval != NULL; }  // Perform SD every x iterations.  
		Ratio* SteepestDescentInterval;
		int steepestDescentInterval;
	
	bool IsSteepestDescentAlways; // max h(x) -> max g(x) -> then penalize  (Always optimize original function g(x) after optimizing h(x)).  
	
	inline bool IsEvaporate() { return EvaporateMode != EvapNone && EvaporateAmount > 0.0; }
		kEvaporateMode EvaporateMode;
		double EvaporateAmount;

	inline bool IsEvaporateOnSwap() { return EvaporateOnSwapScale > 0.0 && EvaporateOnSwapScale <= 1.0; } 
		double EvaporateOnSwapScale; // (0,1]
		
	inline bool IsEvaporateOnInterval() { return EvaporateInterval!=NULL && EvaporateOnIntervalScale > 0.0 && EvaporateOnIntervalScale <= 1.0; } 
		Ratio* EvaporateInterval; 
		int evaporateInterval;
		double EvaporateOnIntervalScale; // (0,1]

	inline bool IsEvaporateSinceImprove() { return EvaporateSinceImproveInterval!=NULL && EvaporateSinceImproveScale > 0.0 && EvaporateSinceImproveScale <= 1.0 && EvaporateSinceImproveDecay <= 1.0 && EvaporateSinceImproveDecay > 0.0; } 
		Ratio* EvaporateSinceImproveInterval; 
		int evaporateSinceImproveInterval;
		double EvaporateSinceImproveScale; // (0,1]
		double EvaporateSinceImproveDecay; // (0,1]


	inline bool IsEvaporateOnImprove() { return EvaporateOnImproveScale > 0.0 && EvaporateOnImproveScale <= 1.0; }  // Amount to scale all penalties by: 1.0 means clear all penalties, .5 means half all of them.
		double EvaporateOnImproveScale; // (0,1] 

	inline bool IsSteepMode() { return Steep.PerturbMax != NULL && Steep.PerturbMin != NULL; }
		SteepGLSParms Steep;

	inline bool IsDynamicLambda() { return LambdaPower >= 1.0; } // r = rand(0,1), lambda=(1-r^power) * lambda.  
		double LambdaPower;

	inline bool IsThreaded() { return Threads > 0; }
		int Threads;
															
	GLSParms() : Lambda(1.0), penaltyAmount(1.0), penaltyMode(IncreaseByUtility), IsAspireBest(true), AspireLate(NULL), RandomMovePr(0.0), BestMoveInterval(NULL), BestMovePr(0.0), PenaltyNoisePr(0.0), SteepestDescentInterval(NULL), IsSteepestDescentAlways(false), EvaporateMode(EvapNone), EvaporateAmount(.9), EvaporateOnImproveScale(0.0), EvaporateOnSwapScale(0.0), EvaporateInterval(NULL), EvaporateOnIntervalScale(0.0), LambdaPower(0.0), Threads(0), AspireGood(NULL),EvaporateSinceImproveInterval(NULL), EvaporateSinceImproveScale(0.0), EvaporateSinceImproveDecay(1.0)
	{
	}

	// My makeshift reflection for Parameter Optimization (Particle Swarm Optimization)
	inline void Set(string property, Ratio& r)
	{
		if (Global::IsEqual(property,"Lambda"))
			Lambda = r.x[0];
		if (Global::IsEqual(property,"EvaporateInterval"))
			EvaporateInterval = &r;
		if (Global::IsEqual(property,"EvaporateOnIntervalScale"))
			EvaporateOnIntervalScale = r.x[0];
	}

	inline void Calculate(int n)
	{
		if (IsAspireLate()) aspireLate = AspireLate->Calculate(n);
		if (IsBestMoveInterval()) bestMoveInterval = BestMoveInterval->Calculate(n);
		if (IsSteepestDescentInterval()) steepestDescentInterval = SteepestDescentInterval->Calculate(n);
		if (IsSteepMode()) Steep.Calculate(n);
		if (IsEvaporateOnInterval()) evaporateInterval = EvaporateInterval->Calculate(n);
		if (IsAspireGood()) aspireGood = AspireGood->Calculate(n);
		if (IsEvaporateSinceImprove()) { evaporateSinceImproveInterval = EvaporateSinceImproveInterval->Calculate(n); }
	}

	inline string Name() { return "GLS"; }
	inline void Append(stringstream& s)
	{
		s << "L=" << Lambda; 
		if (IsDynamicLambda()) s << " L<=" << LambdaPower;  
		s << " Penalize=" << (penaltyMode == IncreaseByUtility ? "util" : penaltyMode == IncreaseAllByUtility ? "allUtil" : penaltyMode == IncreaseByCost ? "cost" : penaltyMode == IncreaseAllByCost ? "allCost" : "") << "(" << penaltyAmount << ")";

		if (IsAspireBest) s << " aspBest";
		if (IsAspireGood()) s << " aspGood=" << AspireGood->ToString(); 
		if (IsAspireLate()) s << " aspLate=" << AspireLate->ToString();
		if (IsBestMoveInterval()) s << " bestMove=" << BestMoveInterval->ToString();
		if (IsBestMovePr()) s << " bestMove=" << BestMovePr;
		if (IsRandomMove()) s << " randMove=" << RandomMovePr;
		if (IsPenaltyNoise()) s << " pNoise=" << PenaltyNoisePr;
		if (IsSteepestDescentInterval()) s << " SD=" << SteepestDescentInterval->ToString();
		if (IsSteepestDescentAlways) s << " SDAlways";
		if (IsEvaporate()) s << " evap=" << (EvaporateMode == EvapByPenalty ? " Worst" : EvaporateMode == EvapAllByPenalty ? " All" : EvaporateMode == EvapByLate ? "Late" : EvaporateMode == EvapAllByLate ? "AllLate" : "") << "(" << EvaporateAmount << ")";
		if (IsEvaporateOnImprove()) s << " evapOnImp=" << EvaporateOnImproveScale;
		if (IsEvaporateOnSwap()) s << " evapOnSwap=" << EvaporateOnSwapScale;
		if (IsEvaporateOnInterval()) s << " evapOnInt=" <<EvaporateOnIntervalScale << ":" << EvaporateInterval->ToString();
		if (IsEvaporateSinceImprove()) s << " evapSinceImp=" << EvaporateSinceImproveScale << ":" << EvaporateSinceImproveInterval->ToString(); if (EvaporateSinceImproveDecay<1.0) s << " decay=" << EvaporateSinceImproveDecay;
		if (IsSteepMode()) Steep.Append(s);
		if (IsThreaded()) s << " threads=" << Threads;
	}

	~GLSParms() 
	{
		delete AspireLate;
		delete BestMoveInterval;
		delete SteepestDescentInterval;
		delete EvaporateInterval;
		delete EvaporateSinceImproveInterval;
		delete AspireGood;
	}
};



class TabuSearchParms : public AlgoParms
{

public:
	// Required
	Ratio *T; // > n^2/2
	Ratio *U; // < n^2/2
	int t, u;

	inline string Name() { return "TS"; }
	inline void Append(stringstream& s)
	{
		s << "u=" << U->ToString() << " t=" << T->ToString();
	}

	TabuSearchParms() : T(NULL), U(NULL) {} 

	inline void Calculate(int n)
	{
		t = T->Calculate(n);
		u = U->Calculate(n);
	}

	~TabuSearchParms() 
	{
		delete T; delete U; 
	}
};


class MyTabuSearchParms : public AlgoParms
{

public:
	// Required
	Ratio *T; // > n^2/2
	Ratio *U; // < n^2/2
	int t, u;

	// u = f(t)
	Equation *UAuto;

	// Dynamic U
	Ratio *UMax;
	double DeltaUPercent; // Percent of distance between min and max u to increase.
	Ratio *IncreaseRate; // Number of iterations of no improvement before triggering increase of u. 
	int increaseRate, uMax, uMin, deltaU;

	// Dynamic T
	Ratio *TMax;
	Ratio *TIncreaseRate;  // Number of iterations before increasing t.
	double TIncreasePercent; // Percent of distance between min and max coreSize to increase.
	int tIncrease, tMin, tMax, tIncreaseRate;
	double tIncreasePercent;

	// Dynamic T with Scoping
	double ScopeMin;
	double ScopeRate; // Percent to move scope towards scopeMin each update
	double scopeRatio, tMaxBest, tMinBest, tAvgBest, scopeSum;
	deque<double> scopeValues;
	deque<int> scopeTs;
	int tDelta; // distance between initial tMax and tMin
	int scope;

	// Dynamic T with accelerating increases
	double TIncreasePercentMax;
	double TIncreasePercentRate; // Percent to move TIncreasePercent to TIncreasePercentMax each update

	// Jolt
	LocalSearch *Jolt;
	Ratio *JoltRate;
	bool JoltBest;
	int joltRate;

	// Construction
	Construction* Constructor;
	Ratio *ConstructRate;  // Number of iterations of no improvement before constructing again.
	int constructRate;

	// Exploit TS
	Ratio* Exploit; // Number of iterations of no improvement of exploit best solution (best solution found during current exploit phase) needed to cease exploitation.
	Ratio* Explore; // Number of iterations of consecutive aspiration pool sizes of 0 needed to go to cease exploration.
	Ratio* Defer; // Number of iterations to defer aspiring during exploitation phase.  
	int exploit, explore, defer, exploreCurrent, exploitCurrent;
	double exploitBest;

	// Core
	Ratio *CoreSize;
	CoreConstruct* CoreConstructor;
	Ratio *CoreWorkPhaseLength;
	Ratio *CorePhaseLength;
	int corePhaseLength, coreWorkPhaseLength, coreSize;

	// Reactive Core
	Ratio *MaxCoreSize;
	double CoreIncreasePercent; // Percent of distance between min and max coreSize to increase.
	int coreIncrease, minCoreSize, maxCoreSize;

	inline string Name() { return "TS"; }
	inline void Append(stringstream& s)
	{
		s << "u=" << (UAuto!=NULL ? UAuto->ToString() : U->ToString()) << " t=" << T->ToString();
		if (UMax != NULL)
			s << " uMax=" << UMax->ToString()  << " u+=" << DeltaUPercent*100 << "%" << " rate=" << IncreaseRate->ToString();

		if (Jolt != NULL)
			s << " jolt=" << Jolt->ToString() << " rate=" << JoltRate->ToString() << " best=" << JoltBest;

		if (Constructor != NULL)
			s << " gen=" << Constructor->ToString() << " rate=" << ConstructRate->ToString();
		
		if (CoreSize != NULL)
		{
			s << " core=" << CoreSize->ToString();
			s << " phase=" << CoreWorkPhaseLength->ToString() << ".." << CorePhaseLength->ToString();
		
			if (MaxCoreSize != NULL)
			{
				s << " coreMax=" << MaxCoreSize->ToString() << " core+=" << CoreIncreasePercent*100 << "%";
			}
		}

		if (TMax != NULL)
		{
			s << " tMax=" << TMax->ToString(); 
			s << " t+=" << TIncreasePercent*100 << "%" << " rate=" << TIncreaseRate->ToString();
			if (ScopeTS())
				s << " scopeMin" <<  ScopeMin*100<< "% rate=" << ScopeRate*100 << "%"; 
			if (TIncreasePercentMax != 0)
				s << " t+=max " << TIncreasePercentMax * 100 << "% tRate+=" << TIncreasePercentRate*100 << "%";
		}

		if (Exploit != NULL)
			s << " exploit=" << Exploit->ToString() << " explore=" << Explore->ToString() << " defer=" << Defer->ToString();
	}

	MyTabuSearchParms() : T(NULL), U(NULL), UAuto(NULL), Jolt(NULL), JoltRate(NULL), JoltBest(false), UMax(NULL), DeltaUPercent(0), IncreaseRate(NULL), Constructor(NULL), ConstructRate(NULL), CoreSize(NULL), MaxCoreSize(NULL), TMax(NULL), Exploit(NULL), ScopeMin(-1), TIncreasePercentMax(0)
	{
		CoreConstructor = new CoreConstruct();
	}

	inline bool ScopeTS() { return ScopeMin != -1 && TMax != NULL; }

	inline void Calculate(int n)
	{
		t = T->Calculate(n);
		u = UAuto != NULL ? UAuto->Calculate(t, n) : U->Calculate(n);

		if (JoltRate != NULL)
			joltRate = JoltRate->Calculate(n);
		if (UMax != NULL)
		{
			uMin = u;
			uMax = UMax->Calculate(n);
			uMax = max(uMax, uMin);
			increaseRate = IncreaseRate->Calculate(n);
			DeltaUPercent = DeltaUPercent < 0 ? 0 : DeltaUPercent > 1 ? 1 : DeltaUPercent;
			deltaU = max(1, int((uMax-u)*DeltaUPercent + .5)); 
		}
		if (Constructor != NULL)
		{
			constructRate = ConstructRate->Calculate(n);
			Constructor->Initialize(n);
		}
		if (CoreSize != NULL)
		{
			coreSize = Global::Constrain(CoreSize->Calculate(n),0,n);
			corePhaseLength = CorePhaseLength->Calculate(n);
			coreWorkPhaseLength = CoreWorkPhaseLength->Calculate(n);
			CoreConstructor->Initialize(n);

			if (MaxCoreSize != NULL)
			{
				minCoreSize = coreSize;
				maxCoreSize = Global::Constrain(MaxCoreSize->Calculate(n), minCoreSize, n);
				CoreIncreasePercent = CoreIncreasePercent < 0 ? 0 : CoreIncreasePercent > 1 ? 1 : CoreIncreasePercent;
				coreIncrease = max(1, int((maxCoreSize-minCoreSize)*CoreIncreasePercent + .5));
			}
		}

		if (TMax != NULL)
		{
			tMin = t;
			tMax = max(tMin, TMax->Calculate(n));
			TIncreasePercent = TIncreasePercent < 0 ? 0: TIncreasePercent > 1 ? 1 : TIncreasePercent;
			tIncreasePercent = TIncreasePercent;
			tIncrease = max(1, int((tMax-tMin)*tIncreasePercent+.5));
			tIncreaseRate = max(1, TIncreaseRate->Calculate(n));
			if (ScopeTS())
			{
				ScopeRate = ScopeRate < 0 ? 0 : ScopeRate > 1 ? 1 : ScopeRate;
				ScopeMin = ScopeMin < 0 ? 0 : ScopeMin > 1 ? 1 : ScopeMin;
				scopeRatio = 1.0;
				tDelta = tMax - tMin;
				scope = max(1,int(scopeRatio * tDelta + .5));
				tAvgBest = Global::Max;
				scopeValues.clear();
				scopeTs.clear();
				scopeSum = 0;
			}
			if (TIncreasePercentMax != 0)
			{
			}
		}

		if (Exploit != NULL)
		{
			exploit = Exploit->Calculate(n);
			explore = Explore->Calculate(n);
			defer = Defer->Calculate(n);
			exploitBest = Global::Max;
			exploreCurrent = 0;
			exploitCurrent = 0;
		}



	}

	~MyTabuSearchParms() 
	{
		delete T; delete U; 
		delete UAuto;
		delete UMax; delete IncreaseRate;
		delete JoltRate; delete Jolt;
		delete ConstructRate; delete Constructor;
		delete CoreSize; delete CoreConstructor; delete CoreWorkPhaseLength; delete CorePhaseLength;
		delete MaxCoreSize; 
		delete TMax; delete TIncreaseRate;
		delete Exploit; delete Explore; delete Defer;
	}


};