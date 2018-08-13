#pragma once
#include <unordered_map>
#include <functional>
#include "../utility/PriorityQueue.h"

template <class Domain, class Node, class TopLevelAction>
class DecisionAlgorithm
{
	typedef typename Domain::State State;
	typedef typename Domain::HashState Hash;

public:
	virtual Node* backup(PriorityQueue<Node*>& open, vector<TopLevelAction>& tlas, Node* start)
	{
	}
};