#include <iostream>
#include <vector>
#include "MyITS.cpp"
#include "LocalSearch.cpp"
#include "Instance.cpp"
#include "MyITSParms.cpp"
#include <assert.h>
#include "Setup.cpp"
#include <limits>

using namespace std;

MTRand Global::Twister;
fstream Global::Log;
int Global::Max = INT_MAX;
int Global::Min = INT_MIN;
int AlgoParms::count = 0;


int main()
{
	Setup setup;
	setup.Run();
	return 0;
}

