#pragma once
#include <iostream>
#include <queue>

using namespace std;

struct ResultContainer
{
    queue<int> path;
	bool solutionFound;
	double solutionCost;
	int nodesGenerated;
	int nodesExpanded;

	ResultContainer()
	{
		solutionFound = false;
		solutionCost = -1;
		nodesGenerated = 0;
		nodesExpanded = 0;
	}

	ResultContainer(const ResultContainer& res)
	{
		solutionFound = res.solutionFound;
		solutionCost = res.solutionCost;
		nodesGenerated = res.nodesGenerated;
		nodesExpanded = res.nodesExpanded;
        path = res.path;
	}

	ResultContainer& operator=(const ResultContainer& rhs)
	{
		if (&rhs == this)
			return *this;
		else
		{
			solutionFound = rhs.solutionFound;
			solutionCost = rhs.solutionCost;
			nodesGenerated = rhs.nodesGenerated;
			nodesExpanded = rhs.nodesExpanded;
            path = rhs.path;
			return *this;
		}
	}
};