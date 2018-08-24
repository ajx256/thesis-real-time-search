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
	AStar(Domain& domain, double lookahead, string sorting)
		: domain(domain), lookahead(lookahead), sortingFunction(sorting)
	{}
	
	void incrementLookahead()
	{
		lookahead++;
	}

	void expand(PriorityQueue<Node*>& open, unordered_map<State, Node*, Hash>& closed, vector<TopLevelAction>& tlas,
		std::function<bool(Node*, unordered_map<State, Node*, Hash>&, PriorityQueue<Node*>&, vector<TopLevelAction>&)> duplicateDetection,
		ResultContainer& res)
	{
		genIndex = tlas.size();

		// First things first, reorder open so it matches our expansion policy needs
		sortOpen(open);

		// This starts at 1, because we had to expand start to get the top level actions
		int expansions = 1;

		// Expand until the limit
		while (!open.empty() && (expansions < lookahead))
		{
			// Pop lowest fhat-value off open
			Node* cur = open.top();
			
			// Check if current node is goal
			if (domain.isGoal(cur->getState()))
			{
				return;
			}

			res.nodesExpanded++;
			expansions++;

			open.pop();
			cur->close();

			// Remove this node from the open list of any TLAs
			tlas[cur->getOwningTLA()].open.remove(cur);

			vector<State> children = domain.successors(cur->getState());
			res.nodesGenerated += children.size();

			State bestChild;
			Cost bestF = numeric_limits<double>::infinity();
			
			for (State child : children)
			{
				Node* childNode = new Node(cur->getGValue() + domain.getEdgeCost(child),
					domain.heuristic(child), domain.distance(child), domain.distanceErr(child), 
					domain.epsilonHGlobal(), domain.epsilonDGlobal(), child, cur, cur->getOwningTLA());

				bool dup = duplicateDetection(childNode, closed, open, tlas);

				if (!dup && childNode->getFValue() < bestF)
				{
					bestF = childNode->getFValue();
					bestChild = child;
				}

				// Duplicate detection
				if (!dup)
				{
					childNode->genIndex = genIndex;
					genIndex++;

					open.push(childNode);
					closed[child] = childNode;

					// Add to open of generating TLA
					tlas[childNode->getOwningTLA()].open.push(childNode);
				}
				else
					delete childNode;
			}

			// Learn one-step error
			if (bestF != numeric_limits<double>::infinity())
			{
				Cost epsD = (1 + domain.distance(bestChild)) - cur->getDValue();
				Cost epsH = (domain.getEdgeCost(bestChild) + domain.heuristic(bestChild)) - cur->getHValue();

				domain.pushEpsilonHGlobal(epsH);
				domain.pushEpsilonDGlobal(epsD);
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
	int genIndex;
};