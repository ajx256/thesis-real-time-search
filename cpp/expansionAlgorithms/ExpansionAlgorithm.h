#pragma once
#include <unordered_map>
#include <functional>
#include "../utility/PriorityQueue.h"

template <class Domain, class Node, class TopLevelAction>
class ExpansionAlgorithm
{
	typedef typename Domain::State State;
	typedef typename Domain::HashState Hash;
	typedef typename Domain::Cost Cost;

public:

	virtual void incrementLookahead()
	{
	}

	virtual void expand(PriorityQueue<Node*>& open, unordered_map<State, Node*, Hash>& closed, vector<TopLevelAction>& tlas,
		std::function<bool(Node*, unordered_map<State, Node*, Hash>&, PriorityQueue<Node*>&, vector<TopLevelAction>&)> duplicateDetection,
		ResultContainer& res)
	{
	}
};