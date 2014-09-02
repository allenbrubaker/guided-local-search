#pragma once

#include <sstream>
#include <math.h>
using namespace std;



class Equation
{
public: 
	virtual int Calculate(int t, int n) = 0;
	virtual string ToString() = 0;
};

class BestFit : public Equation
{
public:
	inline int Calculate(int t, int n)
	{
		double x = double(t) / double(n*n);
		double a = 2.65480690193984;
		double b = 2.01760730895095;
		double c = -4.58908752102872;
		double y = x/(a+b*x+c*sqrt(x));
		return int(y * n + .5); 	
	}

	inline string ToString()
	{
		return "x/(a+b*x+c*sqrt(x))";
	}
};

class Logarithmic : public Equation
{
public:
	inline int Calculate(int t, int n)
	{
		double x = double(t) / double(n*n);
		double y = -5.318*log(x) + 12.372;
		y = y<0.5 ? 0.5 : y;
		return int(y * n + .5); 	
	}

	inline string ToString()
	{
		return "-5.318*ln(x)+12.372";
	}
};

class Exponential : public Equation
{
public:
	inline int Calculate(int t, int n)
	{
		double x = double(t) / double(n*n);
		double y = 14.378*exp(-0.288*x);
		y = y<0.5 ? 0.5 : y;
		return int(y * n + .5); 	
	}

	inline string ToString()
	{
		return "14.378*exp(-0.288*x)";
	}
};

class Power : public Equation
{
public:
	inline int Calculate(int t, int n)
	{
		double x = double(t) / double(n*n);
		double y = 15.168*pow(x,-1.172);
		y = y<0.5 ? 0.5 : y;
		return int(y * n + .5); 	
	}

	inline string ToString()
	{
		return "15.168*x^-1.172";
	}
};

class Linear : public Equation
{
public:
	inline int Calculate(int t, int n)
	{
		double x = double(t) / double(n*n);
		double y = -1.2347*x + 11.806;
		y = y<0.5 ? 0.5 : y;
		return int(y * n + .5); 	
	}

	inline string ToString()
	{
		return "-1.2347*x+11.806";
	}
};

class Ratio
{
public:
	double x[3];
	Ratio(double x0=0.0, double x1=0.0, double x2=0.0) 
	{
		x[0] = x0;
		x[1] = x1;
		x[2] = x2;
	}

	inline int Calculate(double n)
	{
		return int( (x[2]*n + x[1])*n + x[0] + .5);
	}
	inline double ToDouble(double n)
	{
		return (x[2]*n + x[1])*n + x[0];
	}
	static inline Ratio* FromValue(double value, int n)
	{
		double decimal = value - int(value);
		int base = int(value);
		Ratio* ratio = new Ratio();
		ratio->x[2] = base / (n*n);
		base = base % (n*n);
		ratio->x[1] = base / n;
		base = base % n;
		ratio->x[0] = base + decimal;
		return ratio;
	}

	inline string ToString()
	{
		stringstream ss;
		bool output = false;
		if (x[0] != 0)
		{
			ss << x[0];
			output = true; 
		}
		if (x[1] != 0)
		{
			ss << (output?"+":"");
			if (x[1]!=1) 
				ss << x[1];
			ss << "n";
			output = true;
		}
		if (x[2] != 0)
		{
			ss << (output?"+":"");
			if (x[2]!=1) 
				ss << x[2];
			ss << "n2";
			output = true;
		}
		if (!output)
			ss << "0";
		return ss.str();
	}
};