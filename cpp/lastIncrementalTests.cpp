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
#include "domain/TreeWorld.h"

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

	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k1P(world, lookaheadDepth, 1);
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k3P(world, lookaheadDepth, 3);
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k5P(world, lookaheadDepth, 5);
	DFSKBestCsernaBackupPembertonBelief<TreeWorld> k7P(world, lookaheadDepth, 7);
	DFSCsernaBackupPembertonBelief<TreeWorld> pemberton(world, lookaheadDepth);
	DFSCsernaBackup<TreeWorld> cserna(world, lookaheadDepth);
	DFSKBestCsernaBackup<TreeWorld> k1(world, lookaheadDepth, 1);
	DFSKBestCsernaBackup<TreeWorld> k3(world, lookaheadDepth, 3);
	DFSKBestCsernaBackup<TreeWorld> k5(world, lookaheadDepth, 5);
	DFSKBestCsernaBackup<TreeWorld> k7(world, lookaheadDepth, 7);
	DFSBellmanBackup<TreeWorld> bellman(world, lookaheadDepth);
	DFSMiniminBackup<TreeWorld> minimin(world, lookaheadDepth);
	DFSNancyBackup<TreeWorld> nancy(world, lookaheadDepth);

	ResultContainer pembertonRes = pemberton.searchLI();
	ResultContainer k1PRes = k1P.searchLI();
	ResultContainer k3PRes = k3P.searchLI();
	ResultContainer k5PRes = k5P.searchLI();
	ResultContainer k7PRes = k7P.searchLI();
	ResultContainer csernaRes = cserna.searchLI();
	ResultContainer k1Res = k1.searchLI();
	ResultContainer k3Res = k3.searchLI();
	ResultContainer k5Res = k5.searchLI();
	ResultContainer k7Res = k7.searchLI();
	ResultContainer bellmanRes = bellman.searchLI();
	ResultContainer miniminRes = minimin.searchLI();
	ResultContainer nancyRes = nancy.searchLI();

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
