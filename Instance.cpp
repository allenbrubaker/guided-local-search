#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <assert.h>
#include <sstream>
#include <cstdlib>
using namespace std;

class Instance
{
private:
public: 
	double **Flow, **Distance;
	int Size;
	string InstanceName, OptimalAlgorithm;
	double OptimalFitness;
	string Type;

	Instance(string instanceName)
	{
		InstanceName = instanceName;
		ifstream in;
		in.open(("../Data/" + instanceName + ".dat").c_str());
		if (!in.is_open())
		{
			in.open(("Data/" + instanceName + ".dat").c_str());
			if (!in.is_open())
			{
				cerr << "Could not find instance: " << instanceName << endl;
				exit(1);
			}
		}
		in >> Size;
		Flow = new double*[Size];
		Distance = new double*[Size];
		for (int k=0; k<2; ++k)
		{
			double **x = k==0 ? Flow : Distance;
			for (int i=0; i<Size; ++i)
			{
				x[i] = new double[Size];
				for (int j=0; j<Size; ++j)
					in >> x[i][j];
			}
		}
		SetVars();
		in.close();
	}

	~Instance()
	{
		for (int i=0; i<Size; ++i)
		{
			delete [] Flow[i];
			delete [] Distance[i];
		}
		delete [] Flow;
		delete [] Distance;
	}

	string ToString() 
	{ 
		stringstream s;
		s << fixed  << InstanceName << "   (n=" << Size << " opt=" << int(OptimalFitness) << ")";
		return s.str();
	}

	void SetVars()
	{
		string o = InstanceName;
		OptimalFitness = o == "bur26a" ? 5426670 : o == "bur26b" ? 3817852 : o == "bur26c" ? 5426795 : o == "bur26d" ? 3821225 : o == "bur26e" ? 5386879 : o == "bur26f" ? 3782044 : o == "bur26g" ? 10117172 : o == "bur26h" ? 7098658 : o == "chr12a" ? 9552 : o == "chr12b" ? 9742 : o == "chr12c" ? 11156 : o == "chr15a" ? 9896 : o == "chr15b" ? 7990 : o == "chr15c" ? 9504 : o == "chr18a" ? 11098 : o == "chr18b" ? 1534 : o == "chr20a" ? 2192 : o == "chr20b" ? 2298 : o == "chr20c" ? 14142 : o == "chr22a" ? 6156 : o == "chr22b" ? 6194 : o == "chr25a" ? 3796 : o == "els19" ? 17212548 : o == "esc16a" ? 68 : o == "esc16b" ? 292 : o == "esc16c" ? 160 : o == "esc16d" ? 16 : o == "esc16e" ? 28 : o == "esc16f" ? 0 : o == "esc16g" ? 26 : o == "esc16h" ? 996 : o == "esc16i" ? 14 : o == "esc16j" ? 8 : o == "esc32a" ? 130 : o == "esc32b" ? 168 : o == "esc32c" ? 642 : o == "esc32d" ? 200 : o == "esc32e" ? 2 : o == "esc32g" ? 6 : o == "esc32h" ? 438 : o == "esc64a" ? 116 : o == "esc128" ? 64 : o == "had12" ? 1652 : o == "had14" ? 2724 : o == "had16" ? 3720 : o == "had18" ? 5358 : o == "had20" ? 6922 : o == "kra30a" ? 88900 : o == "kra30b" ? 91420 : o == "kra32" ? 88700 : o == "lipa20a" ? 3683 : o == "lipa20b" ? 27076 : o == "lipa30a" ? 13178 : o == "lipa30b" ? 151426 : o == "lipa40a" ? 31538 : o == "lipa40b" ? 476581 : o == "lipa50a" ? 62093 : o == "lipa50b" ? 1210244 : o == "lipa60a" ? 107218 : o == "lipa60b" ? 2520135 : o == "lipa70a" ? 169755 : o == "lipa70b" ? 4603200 : o == "lipa80a" ? 253195 : o == "lipa80b" ? 7763962 : o == "lipa90a" ? 360630 : o == "lipa90b" ? 12490441 : o == "nug12" ? 578 : o == "nug14" ? 1014 : o == "nug15" ? 1150 : o == "nug16a" ? 1610 : o == "nug16b" ? 1240 : o == "nug17" ? 1732 : o == "nug18" ? 1930 : o == "nug20" ? 2570 : o == "nug21" ? 2438 : o == "nug22" ? 3596 : o == "nug24" ? 3488 : o == "nug25" ? 3744 : o == "nug27" ? 5234 : o == "nug28" ? 5166 : o == "nug30" ? 6124 : o == "rou12" ? 235528 : o == "rou15" ? 354210 : o == "rou20" ? 725522 : o == "scr12" ? 31410 : o == "scr15" ? 51140 : o == "scr20" ? 110030 : o == "sko42" ? 15812 : o == "sko49" ? 23386 : o == "sko56" ? 34458 : o == "sko64" ? 48498 : o == "sko72" ? 66256 : o == "sko81" ? 90998 : o == "sko90" ? 115534 : o == "sko100a" ? 152002 : o == "sko100b" ? 153890 : o == "sko100c" ? 147862 : o == "sko100d" ? 149576 : o == "sko100e" ? 149150 : o == "sko100f" ? 149036 : o == "ste36a" ? 9526 : o == "ste36b" ? 15852 : o == "ste36c" ? 8239110 : o == "tai30a" ? 1818146 : o == "tai35a" ? 2422002 : o == "tai40a" ? 3139370 : o == "tai12a" ? 224416 : o == "tai12b" ? 39464925 : o == "tai15a" ? 388214 : o == "tai15b" ? 51765268 : o == "tai17a" ? 491812 : o == "tai20a" ? 703482 : o == "tai20b" ? 122455319 : o == "tai25a" ? 1167256 : o == "tai25b" ? 344355646 : o == "tai30b" ? 637117113 : o == "tai35b" ? 283315445 : o == "tai40b" ? 637250948 : o == "tai50a" ? 4938796 : o == "tai50b" ? 458821517 : o == "tai60a" ? 7205962 : o == "tai60b" ? 608215054 : o == "tai64c" ? 1855928 : o == "tai80a" ? 13499184 : o == "tai80b" ? 818415043 : o == "tai100a" ? 21052466 : o == "tai100b" ? 1185996137 : o == "tai150b" ? 498896643 : o == "tai256c" ? 44759294 : o == "tho30" ? 149936 : o == "tho40" ? 240516 : o == "tho150" ? 8133398 : o == "wil50" ? 48816 : o == "wil100" ? 273038 : throw "Instance not found!";
		OptimalAlgorithm = o == "bur26a" ? "opt" : o == "bur26b" ? "opt" : o == "bur26c" ? "opt" : o == "bur26d" ? "opt" : o == "bur26e" ? "opt" : o == "bur26f" ? "opt" : o == "bur26g" ? "opt" : o == "bur26h" ? "opt" : o == "chr12a" ? "opt" : o == "chr12b" ? "opt" : o == "chr12c" ? "opt" : o == "chr15a" ? "opt" : o == "chr15b" ? "opt" : o == "chr15c" ? "opt" : o == "chr18a" ? "opt" : o == "chr18b" ? "opt" : o == "chr20a" ? "opt" : o == "chr20b" ? "opt" : o == "chr20c" ? "opt" : o == "chr22a" ? "opt" : o == "chr22b" ? "opt" : o == "chr25a" ? "opt" : o == "els19" ? "opt" : o == "esc16a" ? "opt" : o == "esc16b" ? "opt" : o == "esc16c" ? "opt" : o == "esc16d" ? "opt" : o == "esc16e" ? "opt" : o == "esc16f" ? "opt" : o == "esc16g" ? "opt" : o == "esc16h" ? "opt" : o == "esc16i" ? "opt" : o == "esc16j" ? "opt" : o == "esc32a" ? "opt" : o == "esc32b" ? "opt" : o == "esc32c" ? "opt" : o == "esc32d" ? "opt" : o == "esc32e" ? "opt" : o == "esc32g" ? "opt" : o == "esc32h" ? "opt" : o == "esc64a" ? "opt" : o == "esc128" ? "opt" : o == "had12" ? "opt" : o == "had14" ? "opt" : o == "had16" ? "opt" : o == "had18" ? "opt" : o == "had20" ? "opt" : o == "kra30a" ? "opt" : o == "kra30b" ? "opt" : o == "kra32" ? "opt" : o == "lipa20a" ? "opt" : o == "lipa20b" ? "opt" : o == "lipa30a" ? "opt" : o == "lipa30b" ? "opt" : o == "lipa40a" ? "opt" : o == "lipa40b" ? "opt" : o == "lipa50a" ? "opt" : o == "lipa50b" ? "opt" : o == "lipa60a" ? "opt" : o == "lipa60b" ? "opt" : o == "lipa70a" ? "opt" : o == "lipa70b" ? "opt" : o == "lipa80a" ? "opt" : o == "lipa80b" ? "opt" : o == "lipa90a" ? "opt" : o == "lipa90b" ? "opt" : o == "nug12" ? "opt" : o == "nug14" ? "opt" : o == "nug15" ? "opt" : o == "nug16a" ? "opt" : o == "nug16b" ? "opt" : o == "nug17" ? "opt" : o == "nug18" ? "opt" : o == "nug20" ? "opt" : o == "nug21" ? "opt" : o == "nug22" ? "opt" : o == "nug24" ? "opt" : o == "nug25" ? "opt" : o == "nug27" ? "opt" : o == "nug28" ? "opt" : o == "nug30" ? "opt" : o == "rou12" ? "opt" : o == "rou15" ? "opt" : o == "rou20" ? "opt" : o == "scr12" ? "opt" : o == "scr15" ? "opt" : o == "scr20" ? "opt" : o == "sko42" ? "ro-ts" : o == "sko49" ? "ro-ts" : o == "sko56" ? "ro-ts" : o == "sko64" ? "ro-ts" : o == "sko72" ? "ro-ts" : o == "sko81" ? "gen" : o == "sko90" ? "ro-ts" : o == "sko100a" ? "gen" : o == "sko100b" ? "gen" : o == "sko100c" ? "gen" : o == "sko100d" ? "gen" : o == "sko100e" ? "gen" : o == "sko100f" ? "gen" : o == "ste36a" ? "opt" : o == "ste36b" ? "opt" : o == "ste36c" ? "opt" : o == "tai30a" ? "ro-ts" : o == "tai35a" ? "ro-ts" : o == "tai40a" ? "ro-ts" : o == "tai12a" ? "opt" : o == "tai12b" ? "opt" : o == "tai15a" ? "opt" : o == "tai15b" ? "opt" : o == "tai17a" ? "opt" : o == "tai20a" ? "opt" : o == "tai20b" ? "opt" : o == "tai25a" ? "opt" : o == "tai25b" ? "opt" : o == "tai30b" ? "opt" : o == "tai35b" ? "ro-ts" : o == "tai40b" ? "ro-ts" : o == "tai50a" ? "its" : o == "tai50b" ? "ro-ts" : o == "tai60a" ? "ts-2" : o == "tai60b" ? "ro-ts" : o == "tai64c" ? "bandb" : o == "tai80a" ? "its" : o == "tai80b" ? "ro-ts" : o == "tai100a" ? "its" : o == "tai100b" ? "ro-ts" : o == "tai150b" ? "gen-3" : o == "tai256c" ? "ant" : o == "tho30" ? "opt" : o == "tho40" ? "sim-2" : o == "tho150" ? "sim-3" : o == "wil50" ? "sim-2" : o == "wil100" ? "gen" : throw "Instance not found!";
		Type = o == "bur26a" ? "[III]" : o == "bur26b" ? "[III]" : o == "bur26c" ? "[III]" : o == "bur26d" ? "[III]" : o == "bur26e" ? "[III]" : o == "bur26f" ? "[III]" : o == "bur26g" ? "[III]" : o == "bur26h" ? "[III]" : o == "chr12a" ? "[?]" : o == "chr12b" ? "[?]" : o == "chr12c" ? "[?]" : o == "chr15a" ? "[?]" : o == "chr15b" ? "[?]" : o == "chr15c" ? "[?]" : o == "chr18a" ? "[?]" : o == "chr18b" ? "[?]" : o == "chr20a" ? "[?]" : o == "chr20b" ? "[?]" : o == "chr20c" ? "[?]" : o == "chr22a" ? "[?]" : o == "chr22b" ? "[?]" : o == "chr25a" ? "[?]" : o == "els19" ? "[III]" : o == "esc16a" ? "[III]" : o == "esc16b" ? "[III]" : o == "esc16c" ? "[III]" : o == "esc16d" ? "[III]" : o == "esc16e" ? "[III]" : o == "esc16f" ? "[III]" : o == "esc16g" ? "[III]" : o == "esc16h" ? "[III]" : o == "esc16i" ? "[III]" : o == "esc16j" ? "[III]" : o == "esc32a" ? "[III]" : o == "esc32b" ? "[III]" : o == "esc32c" ? "[III]" : o == "esc32d" ? "[III]" : o == "esc32e" ? "[III]" : o == "esc32g" ? "[III]" : o == "esc32h" ? "[III]" : o == "esc64a" ? "[III]" : o == "esc128" ? "[III]" : o == "had12" ? "[?]" : o == "had14" ? "[?]" : o == "had16" ? "[?]" : o == "had18" ? "[?]" : o == "had20" ? "[?]" : o == "kra30a" ? "[III]" : o == "kra30b" ? "[III]" : o == "kra32" ? "[III]" : o == "lipa20a" ? "[?]" : o == "lipa20b" ? "[?]" : o == "lipa30a" ? "[?]" : o == "lipa30b" ? "[?]" : o == "lipa40a" ? "[?]" : o == "lipa40b" ? "[?]" : o == "lipa50a" ? "[?]" : o == "lipa50b" ? "[?]" : o == "lipa60a" ? "[?]" : o == "lipa60b" ? "[?]" : o == "lipa70a" ? "[?]" : o == "lipa70b" ? "[?]" : o == "lipa80a" ? "[?]" : o == "lipa80b" ? "[?]" : o == "lipa90a" ? "[?]" : o == "lipa90b" ? "[?]" : o == "nug12" ? "[II]" : o == "nug14" ? "[II]" : o == "nug15" ? "[II]" : o == "nug16a" ? "[II]" : o == "nug16b" ? "[II]" : o == "nug17" ? "[II]" : o == "nug18" ? "[II]" : o == "nug20" ? "[II]" : o == "nug21" ? "[II]" : o == "nug22" ? "[II]" : o == "nug24" ? "[II]" : o == "nug25" ? "[II]" : o == "nug27" ? "[II]" : o == "nug28" ? "[II]" : o == "nug30" ? "[II]" : o == "rou12" ? "[?]" : o == "rou15" ? "[?]" : o == "rou20" ? "[?]" : o == "scr12" ? "[?]" : o == "scr15" ? "[?]" : o == "scr20" ? "[?]" : o == "sko42" ? "[II]" : o == "sko49" ? "[II]" : o == "sko56" ? "[II]" : o == "sko64" ? "[II]" : o == "sko72" ? "[II]" : o == "sko81" ? "[II]" : o == "sko90" ? "[II]" : o == "sko100a" ? "[II]" : o == "sko100b" ? "[II]" : o == "sko100c" ? "[II]" : o == "sko100d" ? "[II]" : o == "sko100e" ? "[II]" : o == "sko100f" ? "[II]" : o == "ste36a" ? "[III]" : o == "ste36b" ? "[III]" : o == "ste36c" ? "[III]" : o == "tai30a" ? "[I]" : o == "tai35a" ? "[I]" : o == "tai40a" ? "[I]" : o == "tai12a" ? "[I]" : o == "tai12b" ? "[IV]" : o == "tai15a" ? "[I]" : o == "tai15b" ? "[IV]" : o == "tai17a" ? "[I]" : o == "tai20a" ? "[I]" : o == "tai20b" ? "[IV]" : o == "tai25a" ? "[I]" : o == "tai25b" ? "[IV]" : o == "tai30b" ? "[IV]" : o == "tai35b" ? "[IV]" : o == "tai40b" ? "[IV]" : o == "tai50a" ? "[I]" : o == "tai50b" ? "[IV]" : o == "tai60a" ? "[I]" : o == "tai60b" ? "[IV]" : o == "tai64c" ? "[III]" : o == "tai80a" ? "[I]" : o == "tai80b" ? "[IV]" : o == "tai100a" ? "[I]" : o == "tai100b" ? "[IV]" : o == "tai150b" ? "[IV]" : o == "tai256c" ? "[III]" : o == "tho30" ? "[II]" : o == "tho40" ? "[II]" : o == "tho150" ? "[II]" : o == "wil50" ? "[II]" : o == "wil100" ? "[II]" : throw "Instance not found!";
	}
	
	static string TypesDescription()
	{
		stringstream s;
		s << "[I]: Unstructured Uniformly Randomly Generated: Flows and distances randomly generated from uniform distribution. Hardest to solve. Most iterative search methods find solutions within 1-2% of best known relatively quickly. [Tai-a]" << endl;
		s << "[II]: Random Flows on Grids: distances defined by Manhatten distance between grid points. Flows randomly generated not necessarily uniformly. They have multiple global optima (4 or 8). [Nug Sko Wil Tho]" << endl;
		s << "[III]: Real-Life: Stem from practical applications of the QAP. Flow matrices have many 0 entries and other entiries are not uniformly distributed. [Ste Els Bur Kra Tai-c Esc]" << endl;
		s << "[IV]: Non-uniforly Random: Real-life-like. Randomly generated problems generated to resemble distributions found in real-life problems because real-life instances are ""small and not interesting"". [Tai-b]" << endl; 
		return s.str();
	}
};

