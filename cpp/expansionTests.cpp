#include <iostream>
#include <string>
#include <fstream>
#include "RealTimeSearch.h"
#include "decisionAlgorithms/KBestBackup.h"
#include "expansionAlgorithms/AStar.h"
#include "expansionAlgorithms/BreadthFirst.h"
#include "expansionAlgorithms/DepthFirst.h"
#include "expansionAlgorithms/Risk.h"
#include "learningAlgorithms/Ignorance.h"
#include "learningAlgorithms/Dijkstra.h"
#include "domain/TreeWorld.h"
#include "domain/SlidingTilePuzzle.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc > 4 || argc < 3)
	{
		cout << "Wrong number of arguments: ./expansionTests.sh <Domain Type> <expansion limit> <optional: output file> < <domain file>" << endl;
		cout << "Available domains are TreeWorld and SlidingPuzzle" << endl;
		exit(1);
	}

	// Get the lookahead depth
	int lookaheadDepth = stoi(argv[2]);

	// Get the domain type
	string domain = argv[1];

	ResultContainer bfsRes;
	ResultContainer astarRes;
	ResultContainer fhatRes;
	ResultContainer riskRes;
	ResultContainer lsslrtaRes;

	if (domain == "TreeWorld")
	{
		// Make a tree world
		TreeWorld world = TreeWorld(cin);

		RealTimeSearch<TreeWorld> bfs(world, "bfs", "none", "k-best", lookaheadDepth, 1, "normal");
		RealTimeSearch<TreeWorld> astar(world, "a-star", "none", "k-best", lookaheadDepth, 1, "normal");
		RealTimeSearch<TreeWorld> fhat(world, "f-hat", "none", "k-best", lookaheadDepth, 1, "normal");
		RealTimeSearch<TreeWorld> risk(world, "risk", "none", "k-best", lookaheadDepth, 1, "normal");
		RealTimeSearch<TreeWorld> lsslrta(world, "a-star", "none", "minimin", lookaheadDepth);

		bfsRes = bfs.search();
		astarRes = astar.search();
		fhatRes = fhat.search();
		riskRes = risk.search();
		lsslrtaRes = lsslrta.search();
	}
	else if (domain == "SlidingPuzzle")
	{
		// Make a tile puzzle
		SlidingTilePuzzle world = SlidingTilePuzzle(cin);

		RealTimeSearch<SlidingTilePuzzle> bfs(world, "bfs", "learn", "k-best", lookaheadDepth, 1, "normal");
		RealTimeSearch<SlidingTilePuzzle> astar(world, "a-star", "learn", "k-best", lookaheadDepth, 1, "normal");
		RealTimeSearch<SlidingTilePuzzle> fhat(world, "f-hat", "learn", "k-best", lookaheadDepth, 1, "normal");
		RealTimeSearch<SlidingTilePuzzle> risk(world, "risk", "learn", "k-best", lookaheadDepth, 1, "normal");
		RealTimeSearch<SlidingTilePuzzle> lsslrta(world, "a-star", "learn", "minimin", lookaheadDepth);

		astarRes = astar.search();
		riskRes = risk.search();
		fhatRes = fhat.search();
		bfsRes = bfs.search();
		lsslrtaRes = lsslrta.search();
	}
	else
	{
		cout << "Available domains are TreeWorld and SlidingPuzzle" << endl;
		exit(1);
	}

	string result = "{ \"BFS\": " + to_string(bfsRes.solutionCost) + ", \"A*\": " +
		to_string(astarRes.solutionCost) + ", \"F-Hat\": " + to_string(fhatRes.solutionCost) +
		", \"Risk\": " + to_string(riskRes.solutionCost) + ", \"LSS-LRTA*\": " + to_string(lsslrtaRes.solutionCost) +
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
