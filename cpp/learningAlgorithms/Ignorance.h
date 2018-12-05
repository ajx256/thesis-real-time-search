#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include "../utility/PriorityQueue.h"
#include "LearningAlgorithm.h"

using namespace std;

template <class Domain, class Node, class TopLevelAction>
class Ignorance : public LearningAlgorithm<Domain, Node, TopLevelAction>
{
	typedef typename Domain::State State;
	typedef typename Domain::Cost Cost;
	typedef typename Domain::HashState Hash;

public:
	Ignorance()
	{}

	void learn(PriorityQueue<shared_ptr<Node> > open, unordered_map<State, shared_ptr<Node>, Hash> closed)
	{
		// Learning is for fucking nerds
		return;
	}
};