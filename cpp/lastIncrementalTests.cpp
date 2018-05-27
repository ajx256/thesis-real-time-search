#include <iostream>
#include <string>
#include <fstream>
#include "algorithm/KBest.h"
#include "domain/treeWorld.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc > 2)
	{
		cout << "Wrong number of arguments: ./lastIncremental <optional: output file> < <domain file>" << endl;
		exit(1);
	}

	// Make a tree world
	TreeWorld world = TreeWorld(cin);

	KBest<TreeWorld> kBestAlgo(world, 7);

	string result = kBestAlgo.lastIncrementalDecision();

	if (argc < 2)
	{
		cout << result << endl;
	}
	else
	{
		ofstream out(argv[1]);

		out << result;
		out.close();
	}

}
