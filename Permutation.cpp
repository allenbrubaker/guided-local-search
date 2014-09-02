#pragma once

#include "Global.cpp"
using namespace std;

class Permutation
{
protected:
	int* Values;
public:
	int Size;
	Permutation(int size)
	{
		Size = size;
		Values = new int[size];
		Randomize();
	}
	~Permutation() 
	{ 
		delete [] Values; 
	}

	inline int& operator[](int index) { return Values[index]; }
	inline void Swap(int i, int j)
	{
		if (i == j) return;
		int t = Values[i];
		Values[i] = Values[j];
		Values[j] = t;
	}
	// Knuth shuffle: start with any permutation, and go through the positions 1 through n-1
	//  and for each position i swap the element currently there with an arbitrarily chosen
	// element from positions i through n inclusive. Uniform random
	inline void Randomize()
	{
		for (int i=0; i<Size; ++i)
			Values[i] = i;
		int n = Size, j;
		for (int i=0; i<n-1; ++i)
		{
			j = Global::Rand(i,n);
			Swap(i,j);
		}
	}

	inline int Distance(Permutation& p)
	{
		int d = 0;
		for (int i=0; i<Size; ++i)
			if (Values[i]!=p[i])
				++d;
		return d;
	}

	inline int HammingDistance(Permutation& p)
	{
		return Distance(p);
	}

	inline int SignDistance(Permutation& p)
	{
		int d = 0;
		for (int i=0; i<Size; ++i)
			if (Values[i]-p[i] > 0)
				++d;
		return d;
	}

	inline friend ostream& operator<<(ostream& out, Permutation& s)
	{
		for (int i=0;i<s.Size; ++i)
			out << s[i] << " ";
		return out;
	}
};
