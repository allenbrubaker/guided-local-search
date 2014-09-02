#pragma once
#include <math.h>
#include "Global.cpp"
using namespace std;
class Statistics
{
public:
	int Count;
	double Total;
	double Total2; // Sum(x^2)
	double Mean() {return Count == 0 ? 0 : (double)Total/(double)Count;}  // E(x)
	double Mean2() {return Count == 0 ? 0 : (double)Total2/(double)Count;}  // E(x^2)
	double StandardDeviation() {return sqrt(Mean2()-Mean()*Mean());} //  variance = E(x^2) - E(x)^2
	double Max;
	double Min;
	int MaxCount; // Times max was found
	int MinCount; // Times min was found
	Statistics() { Reset(); }
	void Reset() { Count=0;Total=0;Total2=0;Max=Global::Min;Min=Global::Max;MaxCount=0;MinCount=0; }
	void Add(double x)
	{
		Total += x;
		Total2 += x * x;
		++Count;
		if (fabs(Max-x)<1e-6) ++MaxCount;
		if (fabs(Min-x)<1e-6) ++MinCount;
		if (x > Max)
		{
			Max = x;
			MaxCount = 1;
		}
		if (x < Min)
		{
			Min = x;
			MinCount = 1;
		}
	}
};
