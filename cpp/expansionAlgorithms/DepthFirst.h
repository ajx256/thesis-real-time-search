#pragma once
#include <unordered_map>
#include <stack>
#include <functional>
#include "ExpansionAlgorithm.h"
#include "../utility/PriorityQueue.h"
#include"../utility/ResultContainer.h"

using namespace std;

template <class Domain, class Node, class TopLevelAction>
class DepthFirst : public ExpansionAlgorithm<Domain, Node, TopLevelAction>
{
	typedef typename Domain::State State;
	typedef typename Domain::Cost Cost;
	typedef typename Domain::HashState Hash;

public:
	DepthFirst(Domain& domain, double lookahead)
		: domain(domain), lookahead(lookahead)
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

		// Start by shoving everything on open onto the stack...
		while (!open.empty())
		{
			tlas[open.top()->getOwningTLA()].open.remove(open.top());

			// Pairs on stack represent <State's node, the depth it was generated in DFS>
			theStack.push(make_pair(open.top(), 2));
			
			open.pop();
		}

		while (!theStack.empty())
		{
			pair<Node*, int> cur = theStack.top();
			theStack.pop();

			// If this node is a goal, do not expand it. If the current depth is equal to our lookahead depth,
			// do not expand it.
			if (cur.second > lookahead || domain.isGoal(cur.first->getState()))
			{
				// Add this node to open and TLA open
				open.push(cur.first);
				tlas[cur.first->getOwningTLA()].open.push(cur.first);
			}
			else
			{
				cur.first->close();
				res.nodesExpanded++;

				// Expand this node and recurse down for each child
				vector<State> children = domain.successors(cur.first->getState());
				res.nodesGenerated += children.size();
				
				State bestChild;
				Cost bestF = numeric_limits<double>::infinity();

				for (State child : children)
				{
					Node* childNode = new Node(cur.first->getGValue() + domain.getEdgeCost(child),
						domain.heuristic(child), domain.distance(child), domain.distanceErr(child), 
						domain.epsilonHGlobal(), domain.epsilonDGlobal(), child, cur.first, cur.first->getOwningTLA());

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
						closed[child] = childNode;
						theStack.push(make_pair(childNode, cur.second + 1));
					}
					else
						delete childNode;
				}

				// Learn the one-step error
				if (bestF != numeric_limits<double>::infinity())
				{
					Cost epsD = (1 + domain.distance(bestChild)) - cur.first->getDValue();
					Cost epsH = (domain.getEdgeCost(bestChild) + domain.heuristic(bestChild)) - cur.first->getHValue();

					domain.pushEpsilonHGlobal(epsH);
					domain.pushEpsilonDGlobal(epsD);
				}
			}
		}
	}

protected:
	Domain& domain;
	double lookahead;
	stack<pair<Node*, int> > theStack;
	int genIndex;
};