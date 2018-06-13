#pragma once
#include <iostream>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <string>
#include <algorithm>
#include "../utility/DiscreteDistribution.h"
#include "../utility/ResultContainer.h"

using namespace std;

# define M_PI 3.14159265358979323846  /* pi */

template <class D>
struct DFSMiniminBackup
{
	typedef typename D::State State;
	typedef typename D::Cost Cost;

	struct Node
	{
		Cost g;
		Cost h;
		Node* parent;
		State stateRep;
		bool open;
		State topLevelState;

	public:
		Cost getGValue() const { return g; }
		Cost getHValue() const { return h; }
		Cost getFValue() const { return g + h; }
		void setHValue(Cost val) { h = val; }
		void setGValue(Cost val) { g = val; }
		State getState() const { return stateRep; }
		Node* getParent() { return parent; }
		void setParent(Node* p) { parent = p; }
		bool onOpen() { return open; }
		void close() { open = false; }
		void reopen() { open = true; }
		void setTopLevelState(State tls) { topLevelState = tls; }
		State getTopLevelState() { return topLevelState; }

		Node(Cost g, Cost h, State treeNode, Node* parent)
			: g(g), h(h), stateRep(treeNode), parent(parent) 
		{
			if (parent != NULL)
				topLevelState = parent->getTopLevelState();
		}
	};

	struct CompareNodes
	{
		bool operator()(const Node* n1, const Node* n2) const
		{
			// Tie break on heuristic
			if (n1->getFValue() == n2->getFValue())
				return n1->getGValue() > n2->getGValue();
			return n1->getFValue() > n2->getFValue();
		}
	};

	DFSMiniminBackup(D& domain) : domain(domain) {}

	~DFSMiniminBackup()
	{
		// delete all of the nodes from the last expansion phase
		for (typename unordered_map<unsigned long, vector<Node*> >::iterator it = openUclosed.begin(); it != openUclosed.end(); it++)
			for (typename vector<Node*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
				delete *it2;

		openUclosed.clear();
	}

	void generateTopLevelActions(Node* start, int lookahead, ResultContainer& res)
	{
		// The first node to be expanded in any problem is the start node
		// Doing so yields the top level actions
		start->close();
		res.nodesExpanded++;

		vector<State> children = domain.successors(start->getState());
		res.nodesGenerated += children.size();
		for (State child : children)
		{
			Node* childNode = new Node(start->getGValue() + domain.getEdgeCost(child.getSeedOffset()),
				domain.heuristic(child), child, start);
			childNode->setTopLevelState(child);

			// Only push top level actions onto open, so they may be selected later
			openUclosed[childNode->getState().hash()].push_back(childNode);

			// Explore under this TLA
			explore(childNode, 2, lookahead, res);
		}
	}

	void explore(Node* cur, int curDepth, int maxDepth, ResultContainer& res)
	{
		// If this node is a goal, do not expand it. If the current depth is equal to our lookahead depth,
		// do not expand it.
		if (curDepth > maxDepth || domain.isGoal(cur->getState()))
		{
			// If this node has the best f-cost, this will determine our minimin decision
			if (best == NULL || best->getFValue() > cur->getFValue())
			{
				if (best != NULL && openUclosed[best->getState().hash()].end() == find(openUclosed[best->getState().hash()].begin(), openUclosed[best->getState().hash()].end(), best))
					delete best;
				best = cur;
			}
		}
		else
		{
			// Expand this node and recurse down for each child
			vector<State> children = domain.successors(cur->getState());
			res.nodesGenerated += children.size();
			for (State child : children)
			{
				Node* childNode = new Node(cur->getGValue() + domain.getEdgeCost(child.getSeedOffset()),
					domain.heuristic(child), child, cur);

				explore(childNode, curDepth + 1, maxDepth, res);
				
				if (childNode != best && childNode != NULL)
					delete childNode;
			}
		}
	}

	ResultContainer search(int lookahead)
	{
		ResultContainer res;
		res.solutionCost = 0;
		res.nodesExpanded = 0;
		res.nodesGenerated = 0;
		res.solutionFound = false;

		// Get the start node
		Node* start = new Node(0, 0, domain.getStartState(), NULL);

		while (1)
		{
			best = NULL;

			// delete all of the TLAs from the last expansion phase
			for (typename unordered_map<unsigned long, vector<Node*> >::iterator it = openUclosed.begin(); it != openUclosed.end(); it++)
				for (typename vector<Node*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
					if (*it2 != start)
						delete *it2;

			openUclosed.clear();

			if (domain.isGoal(start->getState()))
			{
				// TODO: Solution found, stop timer

				// Calculate path cost and return solution
				calculateCost(start, res);

				delete start;
				openUclosed.clear();

				return res;
			}

			// Expand some nodes until expnasion limit
			generateTopLevelActions(start, lookahead, res);

			if (start != best && start != NULL)
				delete start;

			// Get the new start node
			vector<Node*> potential = openUclosed[best->getTopLevelState().hash()];
			for (Node* n : potential)
			{
				if (n->getState() == best->getTopLevelState())
				{
					if (openUclosed[best->getState().hash()].end() == find(openUclosed[best->getState().hash()].begin(), openUclosed[best->getState().hash()].end(), best))
						delete best;
					start = n;
					break;
				}
			}
		}
		return res;
	}

private:
	D & domain;
	priority_queue<Node*, vector<Node*>, CompareNodes> open;
	unordered_map<unsigned long, vector<Node*> > closed;
	unordered_map<unsigned long, vector<Node*> > openUclosed;
	Node* best = NULL;

	void calculateCost(Node* solution, ResultContainer& res)
	{
		res.solutionFound = true;
		res.solutionCost = solution->getFValue();
	}
};