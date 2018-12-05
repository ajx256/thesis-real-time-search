#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include <queue>
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
	BreadthFirst(Domain& domain, double lookahead)
		: domain(domain), lookahead(lookahead)
	{}

	void incrementLookahead()
	{
		lookahead++;
	}

	void expand(PriorityQueue<shared_ptr<Node> >& open, unordered_map<State, shared_ptr<Node>, Hash>& closed, vector<TopLevelAction>& tlas,
		std::function<bool(shared_ptr<Node>, unordered_map<State, shared_ptr<Node>, Hash>&, PriorityQueue<shared_ptr<Node> >&, vector<TopLevelAction>&)> duplicateDetection,
		ResultContainer& res)
	{
		// Empty any old values out of the queue
		while (!q.empty())
		{
			q.pop();
		}

		// Start by shoving everything on open onto the queue...
		for (shared_ptr<Node> n : open)
		{
			q.push(n);
		}

		int expansions = 1;

		while (!q.empty() && expansions < lookahead)
		{
			// Pop lowest fhat-value off open
			shared_ptr<Node> cur = q.front();
			q.pop();
			
			// Check if current node is goal
			if (domain.isGoal(cur->getState()))
			{
				return;
			}

			res.nodesExpanded++;
			expansions++;

			cur->close();
			open.remove(cur);

			// Remove this node from the open list of any TLAs
			tlas[cur->getOwningTLA()].open.remove(cur);

			vector<State> children = domain.successors(cur->getState());
			res.nodesGenerated += children.size();

			State bestChild;
			Cost bestF = numeric_limits<double>::infinity();

			for (State child : children)
			{
				shared_ptr<Node> childNode = make_shared<Node>(cur->getGValue() + domain.getEdgeCost(child),
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
					q.push(childNode);
					open.push(childNode);
					closed[child] = childNode;

					// Add to open of generating TLA
					tlas[childNode->getOwningTLA()].open.push(childNode);
				}
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

protected:
	Domain & domain;
	double lookahead;
	queue<shared_ptr<Node> > q;
};