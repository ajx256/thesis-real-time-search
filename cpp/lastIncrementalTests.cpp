#include <iostream>
#include <string>
#include <fstream>
#include "algorithm/DFSKBestCsernaBackupPembertonBelief.h"
#include "algorithm/DFSCsernaBackupPembertonBelief.h"
#include "domain/treeWorld.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc > 3 || argc < 2)
	{
		cout << "Wrong number of arguments: ./lastIncremental <lookahead depth> <optional: output file> < <domain file>" << endl;
		exit(1);
	}

	// Get the lookahead depth
	int lookaheadDepth = stoi(argv[1]);

	// Make a tree world
	TreeWorld world = TreeWorld(cin);

	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k1(world, 1);
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k3(world, 3);
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k5(world, 5);
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k7(world, 7);
	DFSCsernaBackupPembertonBelief<TreeWorld> pemberton(world);

	ResultContainer pembertonRes = pemberton.searchLI(lookaheadDepth);
	ResultContainer k1Res = k1.searchLI(lookaheadDepth);
	ResultContainer k3Res = k3.searchLI(lookaheadDepth);
	ResultContainer k5Res = k5.searchLI(lookaheadDepth);
	ResultContainer k7Res = k7.searchLI(lookaheadDepth);

	string result = "{ \"K 1\": " + to_string(k1Res.solutionCost) + ", \"K 3\": " +
		to_string(k3Res.solutionCost) + ", \"K 5\": " + to_string(k5Res.solutionCost) +
		", \"K 7\": " + to_string(k7Res.solutionCost) + ", \"All Frontier Nodes\": " +
		to_string(pembertonRes.solutionCost) + ", \"Lookahead\": " + to_string(lookaheadDepth) + " }";

	if (argc < 3)
	{
		cout << result << endl;
	}
	else
	{
		ofstream out(argv[2]);

		out << result;
		out.close();
	}

}
