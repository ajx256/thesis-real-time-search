#pragma once
#include <unordered_map>
#include <functional>
#include "ExpansionAlgorithm.h"
#include "../utility/PriorityQueue.h"
#include"../utility/ResultContainer.h"

using namespace std;

template <class Domain, class Node, class TopLevelAction>
class AStar : public ExpansionAlgorithm<Domain, Node, TopLevelAction>
{
	typedef typename Domain::State State;
	typedef typename Domain::Cost Cost;
	typedef typename Domain::HashState Hash;

public:
	AStar(Domain& domain, double lookahead, string sorting, Cost eps)
		: domain(domain), lookahead(lookahead), sortingFunction(sorting), eps(eps)
	{}
	
	void incrementLookahead()
	{
		lookahead++;
	}

	void expand(PriorityQueue<Node*>& open, unordered_map<State, Node*, Hash>& closed, vector<TopLevelAction>& tlas,
		std::function<bool(State, Cost, Cost, Cost, Node*, set<int>, unordered_map<State, Node*, Hash>&)> duplicateDetection, ResultContainer& res)
	{
		// First things first, reorder open so it matches our expansion policy needs
		sortOpen(open);

		// This starts at 1, because we had to expand start to get the top level actions
		int expansions = 1;

		// Expand until the limit
		while (!open.empty() && (expansions < lookahead))
		{
			// Pop lowest fhat-value off open
			Node* cur = open.top();
			cur->close();
			
			// Check if current node is goal
			if (domain.isGoal(cur->getState()))
			{
				return;
			}

			res.nodesExpanded++;
			expansions++;

			open.pop();

			// Remove this node from the open list of any TLAs
			for (int tla : cur->getOwningTLAs())
				tlas[tla].open.remove(cur);

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

private:
	void sortOpen(PriorityQueue<Node*>& open)
	{
		if (sortingFunction == "f")
			open.swapComparator(Node::compareNodesF);
		else if (sortingFunction == "fhat")
			open.swapComparator(Node::compareNodesFHat);
	}

protected:
	Domain & domain;
	double lookahead;
	string sortingFunction;
	Cost eps;
};