#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include "../RealTimeSearch.h"

template<class Domain, class Node, class TopLevelAction>
class LearningAlgorithm
{
	typedef typename Domain::State State;
	typedef typename Domain::HashState Hash;

public:
	virtual void learn(PriorityQueue<shared_ptr<Node> > open, unordered_map<State, shared_ptr<Node>, Hash> closed)
	{
	}
};