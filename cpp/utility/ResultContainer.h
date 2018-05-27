#pragma once
#include <iostream>

using namespace std;

struct ResultContainer
{
	bool solutionFound;
	double solutionCost;
	int nodesGenerated;
	int nodesExpanded;
};