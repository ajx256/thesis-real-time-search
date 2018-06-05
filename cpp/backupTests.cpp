#include <iostream>
#include <string>
#include <fstream>
#include "algorithm/DFSMiniminBackup.h"
#include "algorithm/DFSBellmanBackup.h"
#include "algorithm/DFSNancyBackup.h"
#include "algorithm/DFSCsernaBackup.h"
#include "algorithm/DFSCsernaBackupPembertonBelief.h"
#include "domain/treeWorld.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc > 3 || argc < 2)
	{
		cout << "Wrong number of arguments: ./backupTests <lookahead depth> <optional: output file> < <domain file>" << endl;
		exit(1);
	}

	// Get the lookahead depth
	int lookaheadDepth = stoi(argv[1]);

	// Make a tree world
	TreeWorld world = TreeWorld(cin);

	// Run DFS with differing backup methods for decision making
	DFSMiniminBackup<TreeWorld> minimin(world);
	DFSBellmanBackup<TreeWorld> bellman(world);
	DFSNancyBackup<TreeWorld> nancy(world);
	DFSCsernaBackup<TreeWorld> cserna(world);
	DFSCsernaBackupPembertonBelief<TreeWorld> pemberton(world);

	ResultContainer pembertonRes = pemberton.search(lookaheadDepth);
	ResultContainer miniminRes = minimin.search(lookaheadDepth);
	ResultContainer bellmanRes = bellman.search(lookaheadDepth);
	ResultContainer nancyRes = nancy.search(lookaheadDepth);
	ResultContainer csernaRes = cserna.search(lookaheadDepth);

	string result = "{ \"Minimin\": " + to_string(miniminRes.solutionCost) + ", \"Bellman\": " + 
		to_string(bellmanRes.solutionCost) + ", \"Nancy\": " + to_string(nancyRes.solutionCost) + 
		", \"Cserna\": " + to_string(csernaRes.solutionCost) + ", \"CsernaPembertonBelief\": " + 
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
