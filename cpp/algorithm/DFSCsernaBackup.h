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
struct DFSCsernaBackup
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
		State topLevelState;

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
		void setTopLevelState(State tls) { topLevelState = tls; }
		State getTopLevelState() { return topLevelState; }

		Node(Cost g, Cost h, Cost derr, Cost eps, State treeNode, Node* parent, int tla)
			: g(g), h(h), derr(derr), eps(eps), stateRep(treeNode), parent(parent), owningTLA(tla)
		{
			if (parent != NULL)
				topLevelState = parent->getTopLevelState();
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

	DFSCsernaBackup(D& domain) : domain(domain) {}

	~DFSCsernaBackup()
	{
		// delete all of the nodes from the last expansion phase
		for (typename unordered_map<unsigned long, vector<Node*> >::iterator it = openUclosed.begin(); it != openUclosed.end(); it++)
			for (typename vector<Node*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
				delete *it2;

		openUclosed.clear();
	}

	bool duplicateDetection(Node* node)
	{
		// Check if this node exists 
		vector<Node*> potentialDups = openUclosed[node->getState().hash()];
		for (Node* n : potentialDups)
		{
			if (n->getState() == node->getState())
			{
				// These two states are dups, so check if both are on open
				if (n->onOpen())
				{
					// If on open, keep better of two
					if (node->getGValue() < n->getGValue())
					{
						n->setGValue(node->getGValue());
						n->setParent(node->getParent());
						// TODO: Add or remove from worse TLA?
					}
				}
				return true;
			}
		}

		return false;
	}

	void generateTopLevelActions(Node* start, int lookahead, ResultContainer& res)
	{
		// The first node to be expanded in any problem is the start node
		// Doing so yields the top level actions
		start->close();
		closed[start->getState().hash()].push_back(start);
		res.nodesExpanded++;

		vector<State> children = domain.successors(start->getState());
		res.nodesGenerated += children.size();
		for (State child : children)
		{
			Node* childNode = new Node(start->getGValue() + domain.getEdgeCost(child),
				domain.heuristic(child), domain.distance(child), eps, child, start, topLevelActions.size());
			childNode->setTopLevelState(child);
			// No top level action will ever be a duplicate, so no need to check.
			// Make a new top level action and push this node onto its open
			TopLevelAction tla;
			tla.expectedMinimumPathCost = childNode->getFHatValue();
			//tla.topLevelOpen.push(childNode);
			tla.topLevelNode = childNode;
			//open.push(childNode);
			childNode->distribution = DiscreteDistribution(100, childNode->getFValue(), childNode->getFHatValue(), childNode->getD(), (double)(1.0 / (domain.getBranchingFactor() + 1)));
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
			// Add this node to open and recurse back up
			open.push(cur);
			topLevelActions[cur->getOwningTLA()].topLevelOpen.push(cur);
		}
		else
		{
			cur->close();
			// Expand this node and recurse down for each child
			vector<State> children = domain.successors(cur->getState());
			res.nodesGenerated += children.size();
			for (State child : children)
			{
				Node* childNode = new Node(cur->getGValue() + domain.getEdgeCost(child),
					domain.heuristic(child), domain.distance(child), eps, child, cur, cur->getOwningTLA());
				// Duplicate detection
				if (!duplicateDetection(childNode))
				{
					openUclosed[child.hash()].push_back(childNode);
					explore(childNode, curDepth + 1, maxDepth, res);
				}
				else
					delete childNode;
			}
		}
	}

	void csernaBackup(TopLevelAction& tla)
	{
		// We assume in k-best that only the k-best nodes matter.
		if (!tla.kBestNodes.empty())
		{
			// Perform Cserna Backups on k-best nodes
			while (tla.kBestNodes.size() > 1)
			{
				// Take the first two and do a Cserna backup...
				tla.kBestNodes[0]->distribution = tla.kBestNodes[0]->distribution * tla.kBestNodes[1]->distribution;
				// Remove the other used in the backup
				tla.kBestNodes.erase(remove(tla.kBestNodes.begin(), tla.kBestNodes.end(), tla.kBestNodes[1]), tla.kBestNodes.end());
			}

			// Get the expected value of the resulting Cserna Distribution
			tla.expectedMinimumPathCost = tla.kBestNodes[0]->distribution.expectedCost();
		}
		else
		{
			// If nothing was expanded under this TLA, use the expected value of the TLA
			tla.expectedMinimumPathCost = tla.topLevelNode->distribution.expectedCost();
		}
	}

	void kBestDecision()
	{
		// The K-Best decision assumes that the only nodes within the subtrees of the TLAs are the k-best frontier nodes
		// on their opened lists. Find them.
		for (TopLevelAction& tla : topLevelActions)
		{
			int i = 0;
			// Add to the best k nodes while i < k and non-selected nodes exist on the frontier
			while (i < k && !tla.topLevelOpen.empty())
			{
				Node* best = tla.topLevelOpen.top();
				tla.topLevelOpen.pop();

				// Make this node's PDF a discrete distribution...
				best->distribution = DiscreteDistribution(100, best->getFValue(), best->getFHatValue(), best->getD(), (double)(1.0 / (domain.getBranchingFactor() + 1)));

				tla.kBestNodes.push_back(best);
				i++;
			}

			// Now that best of k-best have been pushed up the tree, perform Cserna backup
			csernaBackup(tla);
		}
	}

	ResultContainer searchLI(int lookahead)
	{
		ResultContainer res;
		res.solutionCost = 0;
		res.nodesExpanded = 0;
		res.nodesGenerated = 0;
		res.solutionFound = false;

		// Get the start node
		Node* start = new Node(0, 0, domain.distance(domain.getStartState()), eps, domain.getStartState(), NULL, -1);
		openUclosed[start->getState().hash()].push_back(start);

		// Make the last incremental decision for the first action

		// First, generate the top-level actions
		generateTopLevelActions(start, lookahead, res);

		// Decisison strategy is to go to the top-level action with the minimum expected path cost,
		// based on the distribution of path costs through its k-best children...
		kBestDecision();

		// Take the TLA with the lowest expected minimum path cost
		TopLevelAction lowestExpectedPathTLA = topLevelActions[0];
		for (TopLevelAction tla : topLevelActions)
		{
			if (tla.expectedMinimumPathCost < lowestExpectedPathTLA.expectedMinimumPathCost)
				lowestExpectedPathTLA = tla;
		}

		start = lowestExpectedPathTLA.topLevelNode;

		// clear the TLA list
		topLevelActions.clear();

		// Empty OPEN and CLOSED
		while (!open.empty())
			open.pop();
		closed.clear();

		// delete all of the nodes from the last expansion phase
		for (typename unordered_map<unsigned long, vector<Node*> >::iterator it = openUclosed.begin(); it != openUclosed.end(); it++)
			for (typename vector<Node*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
				if (*it2 != start)
					delete *it2;

		openUclosed.clear();
		openUclosed[start->getState().hash()].push_back(start);

		// Solve optimally with DFS

		exploreLI(start, res);

		start = open.top();

		if (domain.isGoal(start->getState()))
		{
			// TODO: Solution found, stop timer

			// Calculate path cost and return solution
			calculateCost(start, res);

			return res;
		}

		return res;
	}

	void exploreLI(Node* cur, ResultContainer& res)
	{
		// If this node is a goal, do not expand it. If the current depth is equal to our lookahead depth,
		// do not expand it.
		if (domain.isGoal(cur->getState()))
		{
			// Add this node to open and recurse back up
			open.push(cur);
		}
		else
		{
			cur->close();
			// Expand this node and recurse down for each child
			vector<State> children = domain.successors(cur->getState());
			res.nodesGenerated += children.size();
			for (State child : children)
			{
				Node* childNode = new Node(cur->getGValue() + domain.getEdgeCost(child),
					domain.heuristic(child), domain.distance(child), eps, child, cur, cur->getOwningTLA());
				// Duplicate detection
				if (!duplicateDetection(childNode))
				{
					openUclosed[child.hash()].push_back(childNode);
					exploreLI(childNode, res);
				}
				else
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
		openUclosed[start->getState().hash()].push_back(start);

		while (1)
		{
			if (domain.isGoal(start->getState()))
			{
				// TODO: Solution found, stop timer

				// Calculate path cost and return solution
				calculateCost(start, res);

				return res;
			}

			// clear the TLA list
			topLevelActions.clear();

			// Empty OPEN and CLOSED
			while (!open.empty())
				open.pop();
			closed.clear();

			// delete all of the nodes from the last expansion phase
			for (typename unordered_map<unsigned long, vector<Node*> >::iterator it = openUclosed.begin(); it != openUclosed.end(); it++)
				for (typename vector<Node*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
					if (*it2 != start)
						delete *it2;

			openUclosed.clear();
			openUclosed[start->getState().hash()].push_back(start);

			// First, generate the top-level actions
			generateTopLevelActions(start, lookahead, res);

			if (open.empty())
			{
				break;
			}

			// TODO: Learning?

			// Decisison strategy is to go to the top-level action with the minimum expected path cost,
			// based on the distribution of path costs through its k-best children...
			kBestDecision();

			// Take the TLA with the lowest expected minimum path cost
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
	int k = numeric_limits<int>::max();

	void calculateCost(Node* solution, ResultContainer& res)
	{
		res.solutionFound = true;
		res.solutionCost = solution->getFValue();
	}
};