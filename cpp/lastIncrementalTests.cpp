#include <iostream>
#include <string>
#include <fstream>
#include "RealTimeSearch.h"
#include "decisionAlgorithms/ScalarBackup.h"
#include "decisionAlgorithms/KBestBackup.h"
#include "expansionAlgorithms/DepthFirst.h"
#include "learningAlgorithms/Ignorance.h"
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
	
	RealTimeSearch<TreeWorld> k1P(world, "dfs", "none", "k-best", lookaheadDepth, 1, "pemberton");
	RealTimeSearch<TreeWorld> k3P(world, "dfs", "none", "k-best", lookaheadDepth, 3, "pemberton");
	RealTimeSearch<TreeWorld> k5P(world, "dfs", "none", "k-best", lookaheadDepth, 5, "pemberton");
	RealTimeSearch<TreeWorld> k7P(world, "dfs", "none", "k-best", lookaheadDepth, 7, "pemberton");
	RealTimeSearch<TreeWorld> pemberton(world, "dfs", "none", "k-best", lookaheadDepth, numeric_limits<double>::infinity(), "pemberton");
	RealTimeSearch<TreeWorld> k1(world, "dfs", "none", "k-best", lookaheadDepth, 1, "normal");
	RealTimeSearch<TreeWorld> k3(world, "dfs", "none", "k-best", lookaheadDepth, 3, "normal");
	RealTimeSearch<TreeWorld> k5(world, "dfs", "none", "k-best", lookaheadDepth, 5, "normal");
	RealTimeSearch<TreeWorld> k7(world, "dfs", "none", "k-best", lookaheadDepth, 7, "normal");
	RealTimeSearch<TreeWorld> cserna(world, "dfs", "none", "k-best", lookaheadDepth, numeric_limits<double>::infinity(), "normal");
	RealTimeSearch<TreeWorld> bellman(world, "dfs", "none", "bellman", lookaheadDepth);
	RealTimeSearch<TreeWorld> minimin(world, "dfs", "none", "minimin", lookaheadDepth);
	RealTimeSearch<TreeWorld> nancy(world, "dfs", "none", "k-best", lookaheadDepth, 1, "normal");

	ResultContainer pembertonRes = pemberton.lastIncrementalDecision();
	ResultContainer k1PRes = k1P.lastIncrementalDecision();
	ResultContainer k3PRes = k3P.lastIncrementalDecision();
	ResultContainer k5PRes = k5P.lastIncrementalDecision();
	ResultContainer k7PRes = k7P.lastIncrementalDecision();
	ResultContainer csernaRes = cserna.lastIncrementalDecision();
	ResultContainer k1Res = k1.lastIncrementalDecision();
	ResultContainer k3Res = k3.lastIncrementalDecision();
	ResultContainer k5Res = k5.lastIncrementalDecision();
	ResultContainer k7Res = k7.lastIncrementalDecision();
	ResultContainer bellmanRes = bellman.lastIncrementalDecision();
	ResultContainer miniminRes = minimin.lastIncrementalDecision();
	ResultContainer nancyRes = nancy.lastIncrementalDecision();

	string result = "{ \"K-Best 1 Pemberton Belief\": " + to_string(k1PRes.solutionCost) +
		", \"K-Best 3 Pemberton Belief\": " + to_string(k3PRes.solutionCost) +
		", \"K-Best 5 Pemberton Belief\": " + to_string(k5PRes.solutionCost) +
		", \"K-Best 7 Pemberton Belief\": " + to_string(k7PRes.solutionCost) +
		", \"Cserna Pemberton Belief\": " + to_string(pembertonRes.solutionCost) +
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
