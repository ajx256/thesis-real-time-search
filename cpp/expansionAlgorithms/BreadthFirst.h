#pragma once
#include <unordered_map>
#include <queue>
#include <functional>
#include "ExpansionAlgorithm.h"
#include "../utility/PriorityQueue.h"
#include"../utility/ResultContainer.h"

using namespace std;

template <class Domain, class Node, class TopLevelAction>
class BreadthFirst : public ExpansionAlgorithm<Domain, Node, TopLevelAction>
{
	typedef typename Domain::State State;
	typedef typename Domain::Cost Cost;
	typedef typename Domain::HashState Hash;

public:
	BreadthFirst(Domain& domain, double lookahead, Cost eps)
		: domain(domain), lookahead(lookahead), eps(eps)
	{}

	void incrementLookahead()
	{
		lookahead++;
	}

	void expand(PriorityQueue<Node*>& open, unordered_map<State, Node*, Hash>& closed, vector<TopLevelAction>& tlas,
		std::function<bool(State, Cost, Cost, Cost, Node*, set<int>, unordered_map<State, Node*, Hash>&)> duplicateDetection, ResultContainer& res)
	{
		// Empty any old values out of the queue
		while (!q.empty())
		{
			q.pop();
		}

		// Start by shoving everything on open onto the stack...
		while (!open.empty())
		{
			q.push(open.top());
			
			open.pop();
		}

		int expansions = 1;

		while (!q.empty() && expansions < lookahead)
		{
			// Pop lowest fhat-value off open
			Node* cur = q.front();
			q.pop();
			cur->close();

			// Check if current node is goal
			if (domain.isGoal(cur->getState()))
			{
				open.push(cur);
				return;
			}

			res.nodesExpanded++;
			expansions++;

			open.remove(cur);

			// Remove this node from the open list of any TLAs
			for (int tla : cur->getOwningTLAs())
			{
				tlas[tla].open.remove(cur);
			}

			vector<State> children = domain.successors(cur->getState());
			res.nodesGenerated += children.size();
			for (State child : children)
			{
				Node* childNode = new Node(cur->getGValue() + domain.getEdgeCost(child),
					domain.heuristic(child), domain.distance(child), eps, 
					child, cur, cur->getOwningTLAs());

				// Duplicate detection
				if (!duplicateDetection(child, childNode->getGValue(), childNode->getHValue(),
					childNode->getDValue(), cur, childNode->getOwningTLAs(), closed))
				{
					q.push(childNode);
					open.push(childNode);
					closed[child] = childNode;

					// Add to open of TLAs
					for (int tla : childNode->getOwningTLAs())
						tlas[tla].open.push(childNode);
				}
				else
					delete childNode;
			}
		}
	}

protected:
	Domain & domain;
	double lookahead;
	queue<Node*> q;
	Cost eps;
};