#pragma once
#include <unordered_map>
#include <memory>
#include <functional>
#include "../utility/PriorityQueue.h"

template <class Domain, class Node, class TopLevelAction>
class DecisionAlgorithm
{
	typedef typename Domain::State State;
	typedef typename Domain::HashState Hash;

public:
	virtual shared_ptr<Node> backup(PriorityQueue<shared_ptr<Node> >& open, vector<TopLevelAction>& tlas, shared_ptr<Node> start)
	{
	}
};