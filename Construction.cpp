#pragma once

#include "Solution.cpp"
#include "Global.cpp"
#include <list>
#include <typeinfo>
class Construction
{
public:
	inline virtual Solution* Generate(const Instance&, int coreSize=0) = 0;
	inline void UpdateFrom(Solution& s) 
	{
		for (int i = 0; i < s.Size(); i++)
		{
			UpdateFrom(s, i);
		}

	};
	inline virtual void UpdateFrom(Solution&, int) {};
	inline virtual void Initialize(int size) {};

	inline string Name() 
	{
		string name = string(typeid(*this).name());
		int chop = getenv("windir") != NULL ? 6 : 2;
		return name.substr(chop,name.size()-chop-9);
	}
	inline virtual string ToString()
	{
		string p = ParmsToString();
		return "(" + Name() + (p==""?"":":") + p + ")";
	}
	inline virtual string ParmsToString() { return ""; }
};

class RandomConstruct : public Construction
{public:
	inline Solution* Generate(const Instance& x, int coreSize) 
	{
		Solution* s = new Solution(x);
		return s;
	}
};


class EdgeCountConstruct : public Construction
{
	int *Total;
	double **Counts;
public:
	int Size;
	bool isEmpty;
	EdgeCountConstruct() : isEmpty(true), Total(NULL), Size(0), Counts(NULL) {}
	~EdgeCountConstruct() 
	{ 
		if (Counts) 
			Global::DeleteMatrix(Counts, Size);
		delete [] Total;
	}

	inline void Initialize(int size)
	{
		isEmpty = true;
		Global::DeleteMatrix(Counts, Size);
		delete [] Total;

		Size = size;
		Total = new int[size];
		for (int i=0; i<size; ++i)
				Total[i] = 0;
		Counts = Global::CreateMatrix(size,0);
	}


	inline void UpdateFrom(Solution& s, int i)
	{
		isEmpty = false;
		++Total[i];
		++Counts[i][s[i]];
	}

	// Generate novel solutions not seen before by using counts matrix.
	inline Solution* Generate(const Instance& instance)
	{
		Solution* generate = new Solution(instance);
		if (isEmpty)
			return generate;
		int n = Size, r, sum, fitness, total;
		list<int>::iterator j, jMin;
		list<int> candidates; // linked list
		Permutation p(n);

		for (int i=0; i<n; ++i)
			candidates.push_back(i);	

		int column;
		for (int i=0; i<n; ++i)
		{
			column = p[i]; 
			sum = 0;
			total = max(Total[column],1); // sum in some cases might be 0, which messes up everything if all entries are 0 in the current column i.  Thus, make sure Total is positive implying Sum is positive.
			for (j=candidates.begin(); j!=candidates.end(); ++j)
				sum += total - (int)Counts[column][*j];  
			r = Global::Rand(sum)+1; // Spin the wheel -- generate a number in [1,sum]
			for (j=candidates.begin(); j!=candidates.end(); ++j) // Select a random solution of those left in candidates
			{	
				fitness = total - (int)Counts[column][*j];
				r -= fitness; // Subtract a pie weighted by fitness
				if (r <= 0)
					break;
			}
			(*generate)[column] = *j;
			candidates.erase(j);
		}

		return generate;
	}
};


class CoreConstruct : public Construction
{
	int *Total;
	double **Counts;
public:
	int* Core;
	int Size;
	bool isEmpty;
	CoreConstruct() : Core(NULL), isEmpty(true), Total(NULL), Size(0), Counts(NULL) {}
	~CoreConstruct() 
	{ 
		if (Counts) 
			Global::DeleteMatrix(Counts, Size);
		delete [] Total;
		delete [] Core;
	}

	inline void Initialize(int size)
	{
		isEmpty = true;
		Global::DeleteMatrix(Counts, Size);
		delete [] Total;
		delete [] Core;

		Size = size;
		Total = new int[size];
		for (int i=0; i<size; ++i)
				Total[i] = 0;
		Counts = Global::CreateMatrix(size,0);
		Core = new int[size];
	}


	inline void UpdateFrom(Solution& s, int i)
	{
		isEmpty = false;
		++Total[i];
		++Counts[i][s[i]];
	}

	// Generate novel solutions not seen before by using counts matrix.
	inline Solution* Generate(const Instance& instance, int coreSize)
	{
		Solution* generate = new Solution(instance);
		if (isEmpty)
			return generate;
		int n = Size, r, sum, fitness, total;
		list<int>::iterator j, jMin;
		list<int> candidates; // linked list
		Permutation p(n);

		coreSize = Global::Constrain(coreSize, 0, Size);
		for (int i=0; i<Size; ++i)
			candidates.push_back(i);	
		for (int i=0; i<Size; ++i)
			Core[i] = -1;

		// insertion sort - find k minimum columns by column total
		int min, iMin, current;
		for (int i=0; i<coreSize; ++i)
		{
			min = Global::Max;
			for (int j=i; j<Size; ++j)
			{
				current = Total[p[j]]==0 ? Global::Max-1 : Total[p[j]];
				if (current < min)
				{
					min = current;
					iMin = j;
				}
			}
			p.Swap(iMin, i);
		}
		
		// find minimum elements in core columns to find the actual core cells.
		for (int i=0; i<coreSize; ++i)
		{
			min = Global::Max;
			for (j=candidates.begin(); j!=candidates.end(); ++j)
			{
				// iterate through the available cells in the current core column
				current =  Counts[p[i]][*j] == 0 ? Global::Max - 1 : (int)Counts[p[i]][*j];
				if (current < min)
				{
					min = current;
					jMin = j;
				}
			}
			Core[p[i]] = *jMin; // Assign the core cell (facility) to the core column (location)
			(*generate)[p[i]] = *jMin;
			candidates.erase(jMin); // Forbid facility from being assigned to any other location!
		}


		int column;
		for (int i=coreSize; i<n; ++i)
		{
			column = p[i]; 
			sum = 0;
			total = max(Total[column],1); // sum in some cases might be 0, which messes up everything if all entries are 0 in the current column i.  Thus, make sure Total is positive implying Sum is positive.
			for (j=candidates.begin(); j!=candidates.end(); ++j)
				sum += total - (int)Counts[column][*j];  
			r = Global::Rand(sum)+1; // Spin the wheel -- generate a number in [1,sum]
			for (j=candidates.begin(); j!=candidates.end(); ++j) // Select a random solution of those left in candidates
			{	
				fitness = total - (int)Counts[column][*j];
				r -= fitness; // Subtract a pie weighted by fitness
				if (r <= 0)
					break;
			}
			(*generate)[column] = *j;
			candidates.erase(j);
		}

		return generate;
	}

	inline bool IsInCore(int col, int row)
	{
		if (Core != NULL && col < Size && Core[col] == row) return true;
		return false;
	}
};

