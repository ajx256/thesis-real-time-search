#include <iostream>
#include <string>
#include <fstream>
#include "algorithm/AStarMiniminBackup.h"
#include "algorithm/AStarBellmanBackup.h"
#include "algorithm/AStarNancyBackup.h"
#include "algorithm/AStarCsernaBackup.h"
#include "algorithm/AStarCsernaBackupPembertonBelief.h"
#include "algorithm/AStarKBestCsernaBackup.h"
#include "domain/TreeWorld.h"
#include "domain/SlidingTilePuzzle.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc > 4 || argc < 3)
	{
		cout << "Wrong number of arguments: ./backupTestsAS <Domain Type> <lookahead depth> <optional: output file> < <domain file>" << endl;
		cout << "Available domains are TreeWorld and SlidingPuzzle" << endl;
		exit(1);
	}

	// Get the lookahead depth
	int lookaheadDepth = stoi(argv[2]);

	// Get the domain type
	string domain = argv[1];

	ResultContainer miniminRes;
	ResultContainer bellmanRes;
	ResultContainer pembertonRes;
	ResultContainer nancyRes;
	ResultContainer csernaRes;
	ResultContainer k3Res;
	ResultContainer k10Res;
	ResultContainer k30Res;

	if (domain == "TreeWorld")
	{
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

		miniminRes = minimin.search(lookaheadDepth);
		bellmanRes = bellman.search(lookaheadDepth);
		pembertonRes = pemberton.search(lookaheadDepth);
		nancyRes = nancy.search(lookaheadDepth);
		csernaRes = cserna.search(lookaheadDepth);
		k3Res = k3.search(lookaheadDepth);
		k10Res = k10.search(lookaheadDepth);
		k30Res = k30.search(lookaheadDepth);
	}
	else if (domain == "SlidingPuzzle")
	{
		// Make a tile puzzle
		SlidingTilePuzzle world = SlidingTilePuzzle(cin);

		// Run DFS with differing backup methods for decision making
		AStarMiniminBackup<SlidingTilePuzzle> minimin(world);
		AStarBellmanBackup<SlidingTilePuzzle> bellman(world);
		AStarNancyBackup<SlidingTilePuzzle> nancy(world);
		AStarCsernaBackup<SlidingTilePuzzle> cserna(world);
		AStarCsernaBackupPembertonBelief<SlidingTilePuzzle> pemberton(world);
		AStarKBestCsernaBackup<SlidingTilePuzzle> k3(world, 3);
		AStarKBestCsernaBackup<SlidingTilePuzzle> k10(world, 10);
		AStarKBestCsernaBackup<SlidingTilePuzzle> k30(world, 30);

		miniminRes = minimin.search(lookaheadDepth);
		cout << "done minimin" << endl;
		bellmanRes = bellman.search(lookaheadDepth);
		cout << "done bellman" << endl;
		pembertonRes = pemberton.search(lookaheadDepth);
		cout << "done pemberton" << endl;
		nancyRes = nancy.search(lookaheadDepth);
		cout << "done nancy" << endl;
		csernaRes = cserna.search(lookaheadDepth);
		cout << "done cserna" << endl;
		k3Res = k3.search(lookaheadDepth);
		cout << "done k3" << endl;
		k10Res = k10.search(lookaheadDepth);
		cout << "done k10" << endl;
		k30Res = k30.search(lookaheadDepth);
		cout << "done k30" << endl;
	}
	else
	{
		cout << "Available domains are TreeWorld and SlidingPuzzle" << endl;
		exit(1);
	}

	string result = "{ \"Minimin\": " + to_string(miniminRes.solutionCost) + ", \"Bellman\": " +
		to_string(bellmanRes.solutionCost) + ", \"Nancy\": " + to_string(nancyRes.solutionCost) +
		", \"Cserna\": " + to_string(csernaRes.solutionCost) + ", \"Cserna Pemberton Belief\": " +
		to_string(pembertonRes.solutionCost) + ", \"K-Best 3\": " + to_string(k3Res.solutionCost) +
		", \"K-Best 10\": " + to_string(k10Res.solutionCost) + ", \"K-Best 30\": " + to_string(k30Res.solutionCost) +
		", \"Lookahead\": " + to_string(lookaheadDepth) + " }";

	if (argc < 4)
	{
		cout << result << endl;
	}
	else
	{
		ofstream out(argv[3]);

		out << result;
		out.close();
	}

}
