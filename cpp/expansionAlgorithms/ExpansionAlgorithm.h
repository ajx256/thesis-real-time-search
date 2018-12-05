#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
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

	virtual void expand(PriorityQueue<shared_ptr<Node> >& open, unordered_map<State, shared_ptr<Node>, Hash>& closed, vector<TopLevelAction>& tlas,
		std::function<bool(shared_ptr<Node>, unordered_map<State, shared_ptr<Node>, Hash>&, PriorityQueue<shared_ptr<Node> >&, vector<TopLevelAction>&)> duplicateDetection,
		ResultContainer& res)
	{
	}
};