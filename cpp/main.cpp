#include <iostream>
#include <string>
#include <fstream>
//#include "algorithm/LSSLRTAStar.h"
#include "algorithm/RealTimeAStar.h"
#include "algorithm/AStarFHat.h"
#include "algorithm/BeamSearch.h"
#include "algorithm/Nancy.h"
#include "algorithm/KBest.h"
//#include "algorithm/FHat.h"
#include "domain/TreeWorld.h"

using namespace std;

int main(int argc, char** argv)
{
	string algo = "";
	int lookahead = 1;

	if (argc < 3)
	{
		cout << "Wrong number of arguments: ./run.sh <algorithm> <lookahead> <optional: output file> < <domain file>" << endl;
		exit(1);
	}

	if (argc >= 3)
	{
		algo = argv[1];
		lookahead = stoi(argv[2]);
	}

	// Make a tree world
	TreeWorld world = TreeWorld(cin);

	/*if (algo == "lsslrta-star")
	{
		LSSLRTAStar<TreeWorld>::resultContainer solution;
		LSSLRTAStar<TreeWorld> lsslrtaAlgo(world);

		solution = lsslrtaAlgo.search(lookahead);

		// Backtrace the path
		if (solution.solution == NULL)
			cout << "NO SOLUTION" << endl;

		vector<string> path;
		while (solution.solution != NULL)
		{
			path.push_back("DEPTH: " + to_string(solution.solution->getState()->getDepth()) +
				" F-VALUE: " + to_string(solution.solution->getFValue()) + " EDGE COST: " + to_string(solution.solution->getState()->getEdgeCost()));
			solution.solution = solution.solution->getParent();
		}

		if (!path.empty())
		{
			for (vector<string>::reverse_iterator it = path.rbegin(); it != path.rend(); it++)
				cout << *it << endl;
		}

		cout << "Total Path cost: " << solution.solution->getFValue() << endl;
		cout << solution.nodesGen << " nodes generated" << endl;
		cout << solution.nodesExpand << " nodes expanded" << endl;
	}*/
	if (algo == "a-star")
	{
		ResultContainer solution;

		RealTimeAStar<TreeWorld> aStarAlgo(world);

		solution = aStarAlgo.search(lookahead);

		if (argc < 4)
		{
			// Backtrace the path
			if (!solution.solutionFound)
				cout << "NO SOLUTION" << endl;
			else
				cout << "Total Path cost: " << solution.solutionCost << endl;

			cout << solution.nodesGenerated << " nodes generated" << endl;
			cout << solution.nodesExpanded << " nodes expanded" << endl;
		}
		else
		{
			ofstream out(argv[3]);

			string output = "{ \"DomainInfo\": " + world.getDomainInformation() + ", \"Success\": \"" + to_string(solution.solutionFound) + 
				"\", \"SolutionCost\": " + to_string(solution.solutionCost) + ", \"NodesExpanded\": " + to_string(solution.nodesExpanded) + 
				", \"NodesGenerated\": " + to_string(solution.nodesGenerated) + ", \"Lookahead\": " + 
				to_string(lookahead) + ", \"Algorithm\": \"" + algo + "\" }";
			out << output;
			out.close();
		}
	}
	else if (algo == "a-star-f-hat")
	{
		ResultContainer solution;

		AStarFHat<TreeWorld> aStarAlgo(world);

		solution = aStarAlgo.search(lookahead);

		if (argc < 4)
		{
			// Backtrace the path
			if (!solution.solutionFound)
				cout << "NO SOLUTION" << endl;
			else
				cout << "Total Path cost: " << solution.solutionCost << endl;

			cout << solution.nodesGenerated << " nodes generated" << endl;
			cout << solution.nodesExpanded << " nodes expanded" << endl;
		}
		else
		{
			ofstream out(argv[3]);

			string output = "{ \"DomainInfo\": " + world.getDomainInformation() + ", \"Success\": \"" + to_string(solution.solutionFound) +
				"\", \"SolutionCost\": " + to_string(solution.solutionCost) + ", \"NodesExpanded\": " + to_string(solution.nodesExpanded) +
				", \"NodesGenerated\": " + to_string(solution.nodesGenerated) + ", \"Lookahead\": " +
				to_string(lookahead) + ", \"Algorithm\": \"" + algo + "\" }";
			out << output;
			out.close();
		}
	}
	else if (algo == "beam")
	{
		// Read in the max beam width
		if (argc < 4)
		{
			cout << "./run.sh beam <lookahead> <beam width> <optional: output file> < <domain file>" << endl;
			exit(1);
		}

		int beamWidth = stoi(argv[3]);

		ResultContainer solution;

		BeamSearch<TreeWorld> beamAlgo(world, beamWidth);

		solution = beamAlgo.search(lookahead);

		if (argc < 5)
		{
			// Backtrace the path
			if (!solution.solutionFound)
				cout << "NO SOLUTION" << endl;
			else
				cout << "Total Path cost: " << solution.solutionCost << endl;

			cout << solution.nodesGenerated << " nodes generated" << endl;
			cout << solution.nodesExpanded << " nodes expanded" << endl;
		}
		else
		{
			ofstream out(argv[4]);

			string output = "{ \"DomainInfo\": " + world.getDomainInformation() + ", \"Success\": \"" + to_string(solution.solutionFound) +
				"\", \"SolutionCost\": " + to_string(solution.solutionCost) + ", \"NodesExpanded\": " + to_string(solution.nodesExpanded) +
				", \"NodesGenerated\": " + to_string(solution.nodesGenerated) + ", \"Lookahead\": " +
				to_string(lookahead) + ", \"Algorithm\": \"" + algo + "-" + to_string(beamWidth) + "\" }";
			out << output;
			out.close();
		}
	}
	else if (algo == "k-best")
	{
		// Read in the max beam width
		if (argc < 4)
		{
			cout << "./run.sh k-best <lookahead> <k> <optional: output file> < <domain file>" << endl;
			exit(1);
		}

		int k = stoi(argv[3]);

		ResultContainer solution;

		KBest<TreeWorld> kBestAlgo(world, k);

		solution = kBestAlgo.search(lookahead);

		if (argc < 5)
		{
			// Backtrace the path
			if (!solution.solutionFound)
				cout << "NO SOLUTION" << endl;
			else
				cout << "Total Path cost: " << solution.solutionCost << endl;

			cout << solution.nodesGenerated << " nodes generated" << endl;
			cout << solution.nodesExpanded << " nodes expanded" << endl;
		}
		else
		{
			ofstream out(argv[4]);

			string output = "{ \"DomainInfo\": " + world.getDomainInformation() + ", \"Success\": \"" + to_string(solution.solutionFound) +
				"\", \"SolutionCost\": " + to_string(solution.solutionCost) + ", \"NodesExpanded\": " + to_string(solution.nodesExpanded) +
				", \"NodesGenerated\": " + to_string(solution.nodesGenerated) + ", \"Lookahead\": " +
				to_string(lookahead) + ", \"Algorithm\": \"" + algo + "-" + to_string(k) + "\" }";
			out << output;
			out.close();
		}
	}
	else if (algo == "nancy")
	{
		ResultContainer solution;

		Nancy<TreeWorld> nancyAlgo(world);

		solution = nancyAlgo.search(lookahead);

		if (argc < 4)
		{
			// Backtrace the path
			if (!solution.solutionFound)
				cout << "NO SOLUTION" << endl;
			else
				cout << "Total Path cost: " << solution.solutionCost << endl;

			cout << solution.nodesGenerated << " nodes generated" << endl;
			cout << solution.nodesExpanded << " nodes expanded" << endl;
		}
		else
		{
			ofstream out(argv[3]);

			string output = "{ \"DomainInfo\": " + world.getDomainInformation() + ", \"Success\": \"" + to_string(solution.solutionFound) +
				"\", \"SolutionCost\": " + to_string(solution.solutionCost) + ", \"NodesExpanded\": " + to_string(solution.nodesExpanded) +
				", \"NodesGenerated\": " + to_string(solution.nodesGenerated) + ", \"Lookahead\": " +
				to_string(lookahead) + ", \"Algorithm\": \"" + algo + "\" }";
			out << output;
			out.close();
		}
	}
	/*else if (algo == "f-hat")
	{
		FHat<TreeWorld>::resultContainer solution;

		FHat<TreeWorld> fHatAlgo(world);

		solution = fHatAlgo.search(lookahead);

		// Backtrace the path
		if (solution.solution == NULL)
			cout << "NO SOLUTION" << endl;

		vector<string> path;
		while (solution.solution != NULL)
		{
			path.push_back("DEPTH: " + to_string(solution.solution->getState()->getDepth()) +
				" FHAT-VALUE: " + to_string(solution.solution->getFHatValue()) + " EDGE COST: " + to_string(solution.solution->getState()->getEdgeCost()));
			solution.solution = solution.solution->getParent();
		}

		if (!path.empty())
		{
			for (vector<string>::reverse_iterator it = path.rbegin(); it != path.rend(); it++)
				cout << *it << endl;
		}

		cout << "Total Path cost: " << solution.solution->getFValue() << endl;
		cout << solution.nodesGen << " nodes generated" << endl;
		cout << solution.nodesExpand << " nodes expanded" << endl;
	}*/
	else
	{
		cout << "Enter a valid algorithm. Options are: lss-lrta-star, a-star, a-star-f-hat, beam, f-hat, nancy, k-best" << endl;
		exit(1);
	}

	//world.printWorld();
}