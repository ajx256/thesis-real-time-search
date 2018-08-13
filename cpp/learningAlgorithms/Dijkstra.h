#pragma once
#include <unordered_map>
#include <functional>
#include "../utility/PriorityQueue.h"
#include "LearningAlgorithm.h"

using namespace std;

template <class Domain, class Node, class TopLevelAction>
class Dijkstra : public LearningAlgorithm<Domain, Node, TopLevelAction>
{
	typedef typename Domain::State State;
	typedef typename Domain::Cost Cost;
	typedef typename Domain::HashState Hash;

public:
	Dijkstra(Domain& domain)
		: domain(domain)
	{}

	void learn(PriorityQueue<Node*>& open, unordered_map<State, Node*, Hash>& closed)
	{
		// Start by initializing every state in closed to inf h
		for (typename unordered_map<State, Node*, Hash>::iterator it = closed.begin(); it != closed.end(); it++)
		{
			domain.updateHeuristic(it->first, numeric_limits<double>::infinity());
		}

		// Order open by h
		open.swapComparator(Node::compareNodesH);

		// Perform reverse dijkstra while closed is not empy
		while (!closed.empty())
		{
			Node* cur = open.top();
			open.pop();

			closed.erase(cur->getState());

			// Now get all of the predecessors of cur
			for (State s : domain.predecessors(cur->getState()))
			{
				typename unordered_map<State, Node*, Hash>::iterator it = closed.find(s);
				if (it != closed.end() &&
					domain.heuristic(s) > domain.getEdgeCost(s) + domain.heuristic(cur->getState()))
				{
					// Update the heuristic of this pedecessor
					domain.updateHeuristic(s, domain.getEdgeCost(s) + domain.heuristic(cur->getState()));
					// Update the distance of this predecessor
					domain.updateDistance(s, domain.distance(cur->getState()));

					it->second->setDValue(domain.distance(s));
					it->second->setHValue(domain.heuristic(s));

					if (open.find(it->second) != open.end())
					{
						open.push(it->second);
					}
					else
					{
						open.update();
					}
				}
			}
		}
	}
	
protected:
	Domain & domain;
};