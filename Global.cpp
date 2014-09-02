#pragma once
#include "MersenneTwister.cpp"
#include <assert.h>
#include <fstream>
#include <time.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;

class Global
{
	static MTRand Twister;
public:
	static fstream Log;

	static int Max;
	static int Min;

	static bool FileExists(string filename)
	{
	  ifstream ifile(filename.c_str());
	  return ifile;
	}

	// case insensitive string comparison
	static bool IsEqual(string s1, string s2)
	{
		if (s1.length() != s2.length()) return false;
		for (int i=0; i<s1.size(); ++i)
			if (tolower(s1[i]) != tolower(s2[i]))
				return false;
		return true;
	}

	static void OpenFile(string fileName, fstream& file, bool append=false)
	{
		if (file.is_open()) file.close();
		if (fileName == "") fileName = Global::UniqueFileName();
		string filePath = "../Results/" + fileName + ".csv";
		if (!append && Global::FileExists(filePath))
		{
			cerr << "File already exists: " << filePath << endl;
			exit(1);
		}
		if (!append)
			file.open(filePath.c_str(), ios::out);
		else
			file.open(filePath.c_str(), ios::app);
		if (!file.is_open())
		{
			cerr << "File cannot be open." << endl;
			exit(1);
		}
		file << fixed << setprecision(3);	
	}

	static string UniqueFileName()
	{
		stringstream s;
		time_t n = time(NULL);
		tm* now = gmtime(&n);
		s << "Results_" << now->tm_mon+1 <<  "." << now->tm_mday << "_" << now->tm_hour << "." << now->tm_min << "." << now->tm_sec;
		return s.str();
	}


	// Integer between [0,n-1] -- twister uses inclusive but it makes more sense to make it exclusive.
	static inline int Rand(int n) { return Twister.randInt(n <= 0 ? 0 : n-1); }
	// Integer between [incLB, excUB)
	static inline int Rand(int incLB, int excUB) { return incLB + Global::Rand(excUB-incLB);}
	// Random between [0,1]
	static inline double Rand() { return Twister.rand(); }
	// Random double betwen [incMin, incMax]
	static inline double RandDouble(double incMin, double incMax)
	{
		return incMin + Rand() * (incMax-incMin);
	}

	static inline int Constrain(int value, int minValue, int maxValue)
	{
		assert(minValue <= maxValue);
		return value = max(minValue, min(value, maxValue));
	}
	// m <-- n
	static inline void CopyMatrix(double** m, double **n, int rows)
	{
		for (int i=0; i<rows; ++i)
			for (int j=0; j<rows; ++j)
				m[i][j] = n[i][j];
	}

	static inline void DeleteMatrix(double**& m, int rows)
	{
		if (m == NULL) return;
		for (int i=0; i<rows; ++i)
			delete[] m[i];
		delete [] m;
	}
	static inline double** CreateMatrix(int n, double initialValue=Global::Max)
	{
		double** m = new double*[n];
		for (int i=0; i<n; ++i)
		{
			m[i] = new double[n];
			if (initialValue != Global::Max)
				for (int j=0;j<n; ++j)
					m[i][j] = initialValue;
		}

		return m;
	}

	static inline void FindMin(double **m, int rows, bool symmetric, int &iBest, int &jBest)
	{
		double best = Global::Max;
		for (int i=0; i<rows; ++i)
			for (int j=(symmetric?i:0); j<rows; ++j)
			{
				if (m[i][j] < best)
				{
					iBest = i;
					jBest = j;
					best = m[i][j];
				}
			}
	}

	static inline void PrintMatrix(double** m, int rows)
	{
		for (int i=0; i<rows; ++i)
		{
			for (int j=i+1; j<rows; ++j)
				cout << m[i][j] << " ";
			cout << endl;
		}
	}

	// Round roulette proportionality selection using inverted weights and normalizing all weights between [1,infinity first). 
	// Those that have lowest value get highest weight. 
	static inline void InverseRoulette(double** m, int n, bool symmetric, double best, double power, int *choiceI, int *choiceJ)
	{
		double sum = 0;
		double minNum = Global::Max, maxNum = Global::Min;
		int count = 0, minI, minJ;
		for (int i=0; i<n; ++i)
			for (int j=symmetric?i+1:0; j<n; ++j)
			{
				++count;
				sum += m[i][j];
				if (m[i][j] < minNum)
				{
					minNum = m[i][j]; minI = i; minJ = j;
				}
				maxNum = max(maxNum, m[i][j]);
			}

		// do roulette only if we couldn't find a better best solution!
		if (minNum < best)
		{
			*choiceI = minI; *choiceJ = minJ; best = minNum; return;	
		}

		double offset = (minNum < 0 ? -minNum : 0) + 1;  // Normalize between [1, infinity)
		sum = (maxNum * count - sum);  // invert
		sum += offset * count; // offset
		
		double normalSum = 0;
		for (int i=0; i<n; ++i)
			for (int j=symmetric?i+1:0; j<n; ++j)
				normalSum += pow((maxNum - m[i][j] + offset)/sum, power);

		// Roulette
		double roll = Rand()*normalSum;
		
		for (int i=0; i<n; ++i)
			for (int j=symmetric?i+1:0; j<n; ++j)
			{
				roll -= pow((maxNum - m[i][j] + offset)/sum, power);
				if (roll <= 1e-6)
				{
					*choiceI = i; *choiceJ = j; 
					return;
				}
			}
		assert(false);
	}

};	
