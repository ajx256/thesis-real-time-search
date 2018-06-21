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
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k10(world, 10);
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k100(world, 100);
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k1000(world, 1000);
	DFSCsernaBackupPembertonBelief<TreeWorld> pemberton(world);

	ResultContainer pembertonRes = pemberton.searchLI(lookaheadDepth);
	ResultContainer k1Res = k1.searchLI(lookaheadDepth);
	ResultContainer k10Res = k10.searchLI(lookaheadDepth);
	ResultContainer k100Res = k100.searchLI(lookaheadDepth);
	ResultContainer k1000Res = k1000.searchLI(lookaheadDepth);

	string result = "{ \"K 1\": " + to_string(k1Res.solutionCost) + ", \"K 10\": " +
		to_string(k10Res.solutionCost) + ", \"K 100\": " + to_string(k100Res.solutionCost) +
		", \"K 1000\": " + to_string(k1000Res.solutionCost) + ", \"All Frontier Nodes\": " +
		to_string(pembertonRes.solutionCost) + ", \"Lookahead\": " + to_string(lookaheadDepth) + " }";

	if (argc < 3)
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
