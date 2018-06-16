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
struct DFSNancyBackup
{
	typedef typename D::State State;
	typedef typename D::Cost Cost;

	Cost eps = 1.0 / (domain.getBranchingFactor() + 1);

	struct Node
	{
		Cost g;
		Cost h;
		Cost derr;
		Cost eps;
		Node* parent;
		State stateRep;
		int owningTLA;
		bool open;
		DiscreteDistribution distribution;

	public:
		Cost getGValue() const { return g; }
		Cost getHValue() const { return h; }
		Cost getFValue() const { return g + h; }
		Cost getFHatValue() const { return g + h + (derr * eps); }
		void setHValue(Cost val) { h = val; }
		void setGValue(Cost val) { g = val; }
		State getState() const { return stateRep; }
		Node* getParent() { return parent; }
		int getOwningTLA() { return owningTLA; }
		Cost getDError() { return derr * eps; }
		Cost getD() { return derr; }
		void setParent(Node* p) { parent = p; }
		bool onOpen() { return open; }
		void close() { open = false; }
		void reopen() { open = true; }
		
		Node(Cost g, Cost h, Cost derr, Cost eps, State treeNode, Node* parent, int tla)
			: g(g), h(h), derr(derr), eps(eps), stateRep(treeNode), parent(parent), owningTLA(tla)
		{
		}
	};

	struct CompareNodes
	{
		bool operator()(const Node* n1, const Node* n2) const
		{
			// Tie break on f-value
			if (n1->getFValue() == n2->getFValue())
				return n1->getGValue() > n2->getGValue();
			return n1->getFValue() > n2->getFValue();
		}
	};

	struct CompareNodesFHat
	{
		bool operator()(const Node* n1, const Node* n2) const
		{
			// Tie break on f-value
			if (n1->getFHatValue() == n2->getFHatValue())
				return n1->getFValue() > n2->getFValue();
			return n1->getFHatValue() > n2->getFHatValue();
		}
	};

	struct CompareDistance
	{
		bool operator()(Node* n1, Node* n2)
		{
			return n1->getParent()->getD() < n2->getParent()->getD();
		}
	};

	struct TopLevelAction
	{
		priority_queue<Node*, vector<Node*>, CompareNodesFHat> topLevelOpen;
		Cost expectedMinimumPathCost;
		Node* topLevelNode;
		vector<Node*> kBestNodes;
	};

	DFSNancyBackup(D& domain) : domain(domain) {}

	~DFSNancyBackup()
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
				domain.heuristic(child), domain.distance(child), eps, child, start, topLevelActions.size());

			// No top level action will ever be a duplicate, so no need to check.
			// Make a new top level action and push this node onto its open
			TopLevelAction tla;
			tla.expectedMinimumPathCost = -1;
			//tla.topLevelOpen.push(childNode);
			tla.topLevelNode = childNode;
			//open.push(childNode);
			childNode->distribution = DiscreteDistribution(1000, childNode->getFValue(), childNode->getFHatValue(), childNode->getD(), (double)(1.0 / (domain.getBranchingFactor() + 1)));
			// Push this node onto open
			openUclosed[childNode->getState().hash()].push_back(childNode);
			// Add this top level action to the list
			topLevelActions.push_back(tla);

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
			// If this node has a better expected value than the current top level belief, make this the new top level belief
			if (topLevelActions[cur->getOwningTLA()].expectedMinimumPathCost > cur->distribution.expectedCost() || 
				topLevelActions[cur->getOwningTLA()].expectedMinimumPathCost == -1)
				topLevelActions[cur->getOwningTLA()].expectedMinimumPathCost = cur->distribution.expectedCost();
		}
		else
		{
			// Expand this node and recurse down for each child
			vector<State> children = domain.successors(cur->getState());
			res.nodesGenerated += children.size();
			for (State child : children)
			{
				Node* childNode = new Node(cur->getGValue() + domain.getEdgeCost(child.getSeedOffset()),
					domain.heuristic(child), domain.distance(child), eps, child, cur, cur->getOwningTLA());
				
				childNode->distribution = DiscreteDistribution(1000, childNode->getFValue(), childNode->getFHatValue(), childNode->getD(), (double)(1.0 / (domain.getBranchingFactor() + 1)));

				explore(childNode, curDepth + 1, maxDepth, res);

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
		Node* start = new Node(0, 0, domain.distance(domain.getStartState()), eps, domain.getStartState(), NULL, -1);

		while (1)
		{
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

				return res;
			}

			// clear the TLA list
			topLevelActions.clear();

			// First, generate the top-level actions
			generateTopLevelActions(start, lookahead, res);

			// TODO: Learning?

			// Take the TLA with the lowest expected minimum path cost
			for (TopLevelAction& tla : topLevelActions)
			{
				if (tla.expectedMinimumPathCost == -1)
					tla.expectedMinimumPathCost = tla.topLevelNode->getFHatValue();
			}

			TopLevelAction lowestExpectedPathTLA = topLevelActions[0];

			for (TopLevelAction tla : topLevelActions)
			{
				if (tla.expectedMinimumPathCost < lowestExpectedPathTLA.expectedMinimumPathCost)
					lowestExpectedPathTLA = tla;
			}

			start = lowestExpectedPathTLA.topLevelNode;
		}
		return res;
	}

private:
	D & domain;
	vector<TopLevelAction> topLevelActions;
	priority_queue<Node*, vector<Node*>, CompareNodes> open;
	unordered_map<unsigned long, vector<Node*> > closed;
	unordered_map<unsigned long, vector<Node*> > openUclosed;
	int k = 1;

	void calculateCost(Node* solution, ResultContainer& res)
	{
		res.solutionFound = true;
		res.solutionCost = solution->getFValue();
	}
};