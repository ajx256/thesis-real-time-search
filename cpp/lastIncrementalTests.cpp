#include <iostream>
#include <string>
#include <fstream>
#include "algorithm/DFSKBestCsernaBackupPembertonBelief.h"
#include "algorithm/DFSCsernaBackupPembertonBelief.h"
#include "algorithm/DFSBellmanBackup.h"
#include "algorithm/DFSCsernaBackup.h"
#include "algorithm/DFSKBestCsernaBackup.h"
#include "algorithm/DFSMiniminBackup.h"
#include "algorithm/DFSNancyBackup.h"
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

	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k1P(world, 1);
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k3P(world, 3);
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k5P(world, 5);
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k7P(world, 7);
	DFSCsernaBackupPembertonBelief<TreeWorld> pemberton(world);
	DFSCsernaBackup<TreeWorld> cserna(world);
	DFSKBestCsernaBackup<TreeWorld> k1(world, 1);
	DFSKBestCsernaBackup<TreeWorld> k3(world, 3);
	DFSKBestCsernaBackup<TreeWorld> k5(world, 5);
	DFSKBestCsernaBackup<TreeWorld> k7(world, 7);
	DFSBellmanBackup<TreeWorld> bellman(world);
	DFSMiniminBackup<TreeWorld> minimin(world);
	DFSNancyBackup<TreeWorld> nancy(world);

	ResultContainer pembertonRes = pemberton.searchLI(lookaheadDepth);
	ResultContainer k1PRes = k1P.searchLI(lookaheadDepth);
	ResultContainer k3PRes = k3P.searchLI(lookaheadDepth);
	ResultContainer k5PRes = k5P.searchLI(lookaheadDepth);
	ResultContainer k7PRes = k7P.searchLI(lookaheadDepth);
	ResultContainer csernaRes = cserna.searchLI(lookaheadDepth);
	ResultContainer k1Res = k1.searchLI(lookaheadDepth);
	ResultContainer k3Res = k3.searchLI(lookaheadDepth);
	ResultContainer k5Res = k5.searchLI(lookaheadDepth);
	ResultContainer k7Res = k7.searchLI(lookaheadDepth);
	ResultContainer bellmanRes = bellman.searchLI(lookaheadDepth);
	ResultContainer miniminRes = minimin.searchLI(lookaheadDepth);
	ResultContainer nancyRes = nancy.searchLI(lookaheadDepth);

	string result = "{ \"K-Best 1 Pemberton Belief\": " + to_string(k1PRes.solutionCost) + 
		", \"K-Best 3 Pemberton Belief\": " + to_string(k3PRes.solutionCost) + 
		", \"K-Best 5 Pemberton Belief\": " + to_string(k5PRes.solutionCost) +
		", \"K-Best 7 Pemberton Belief\": " + to_string(k7PRes.solutionCost) + 
		", \"Cserna Pemberton Belief\": " +	to_string(pembertonRes.solutionCost) + 
		", \"K-Best 1\": " + to_string(k1Res.solutionCost) + 
		", \"K-Best 3\": " + to_string(k3Res.solutionCost) + 
		", \"K-Best 5\": " + to_string(k5Res.solutionCost) + 
		", \"K-Best 7\": " + to_string(k7Res.solutionCost) + 
		", \"Cserna\": " + to_string(csernaRes.solutionCost) + 
		", \"Bellman\": " + to_string(bellmanRes.solutionCost) + 
		", \"Minimin\": " + to_string(miniminRes.solutionCost) + 
		", \"Nancy\": " + to_string(nancyRes.solutionCost) + 
		", \"Lookahead\": " + to_string(lookaheadDepth) + " }";

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
