#include <iostream>
#include <string>
#include <fstream>
#include "algorithm/AStarMiniminBackup.h"
#include "algorithm/AStarBellmanBackup.h"
#include "algorithm/AStarNancyBackup.h"
#include "algorithm/AStarCsernaBackup.h"
#include "algorithm/AStarCsernaBackupPembertonBelief.h"
#include "algorithm/AStarKBestCsernaBackup.h"
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
	AStarMiniminBackup<TreeWorld> minimin(world);
	AStarBellmanBackup<TreeWorld> bellman(world);
	AStarNancyBackup<TreeWorld> nancy(world);
	AStarCsernaBackup<TreeWorld> cserna(world);
	AStarCsernaBackupPembertonBelief<TreeWorld> pemberton(world);
	AStarKBestCsernaBackup<TreeWorld> k3(world, 3);
	AStarKBestCsernaBackup<TreeWorld> k10(world, 10);
	AStarKBestCsernaBackup<TreeWorld> k30(world, 30);

	ResultContainer miniminRes = minimin.search(lookaheadDepth);
	ResultContainer bellmanRes = bellman.search(lookaheadDepth);
	ResultContainer pembertonRes = pemberton.search(lookaheadDepth);
	ResultContainer nancyRes = nancy.search(lookaheadDepth);
	ResultContainer csernaRes = cserna.search(lookaheadDepth);
	ResultContainer k3Res = k3.search(lookaheadDepth);
	ResultContainer k10Res = k10.search(lookaheadDepth);
	ResultContainer k30Res = k30.search(lookaheadDepth);


	string result = "{ \"Minimin\": " + to_string(miniminRes.solutionCost) + ", \"Bellman\": " +
		to_string(bellmanRes.solutionCost) + ", \"Nancy\": " + to_string(nancyRes.solutionCost) +
		", \"Cserna\": " + to_string(csernaRes.solutionCost) + ", \"Cserna Pemberton Belief\": " +
		to_string(pembertonRes.solutionCost) + ", \"K-Best 3\": " + to_string(k3Res.solutionCost) +
		", \"K-Best 10\": " + to_string(k10Res.solutionCost) + ", \"K-Best 30\": " + to_string(k30Res.solutionCost) +
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
