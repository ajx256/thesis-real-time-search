#include <iostream>
#include <string>
#include <fstream>
#include "algorithm/BFSNancyBackup.h"
#include "algorithm/FHatNancyBackup.h"
#include "algorithm/AStarNancyBackup.h"
#include "algorithm/RiskNancyBackup.h"
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

	if (domain == "TreeWorld")
	{
		// Make a tree world
		TreeWorld world = TreeWorld(cin);

		BFSNancyBackup<TreeWorld> bfs(world);
		AStarNancyBackup<TreeWorld> astar(world);
		FHatNancyBackup<TreeWorld> fhat(world);
		RiskNancyBackup<TreeWorld> risk(world, 1);

		bfsRes = bfs.search(lookaheadDepth);
		astarRes = astar.search(lookaheadDepth);
		fhatRes = fhat.search(lookaheadDepth);
		riskRes = risk.search(lookaheadDepth);
	}
	else if (domain == "SlidingPuzzle")
	{
		// Make a tile puzzle
		SlidingTilePuzzle world = SlidingTilePuzzle(cin);

		BFSNancyBackup<SlidingTilePuzzle> bfs(world);
		AStarNancyBackup<SlidingTilePuzzle> astar(world);
		FHatNancyBackup<SlidingTilePuzzle> fhat(world);
		RiskNancyBackup<SlidingTilePuzzle> risk(world, 1);

		bfsRes = bfs.search(lookaheadDepth);
		astarRes = astar.search(lookaheadDepth);
		fhatRes = fhat.search(lookaheadDepth);
		riskRes = risk.search(lookaheadDepth);
	}
	else
	{
		cout << "Available domains are TreeWorld and SlidingPuzzle" << endl;
		exit(1);
	}

	string result = "{ \"BFS\": " + to_string(bfsRes.solutionCost) + ", \"A*\": " +
		to_string(astarRes.solutionCost) + ", \"F-Hat\": " + to_string(fhatRes.solutionCost) +
		", \"Risk\": " + to_string(riskRes.solutionCost) + ", \"Lookahead\": " + to_string(lookaheadDepth) + " }";

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