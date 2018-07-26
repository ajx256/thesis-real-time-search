#pragma once
#include <iostream>
#include <queue>
#include <unordered_map>
#include "../utility/ResultContainer.h"

using namespace std;

template <class D>
struct AStarFHat
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
		bool open;

	public:
		Cost getGValue() const { return g; }
		Cost getHValue() const { return h; }
		Cost getFValue() const { return g + h; }
		Cost getFHatValue() const { return g + h + (derr * eps); }
		void setHValue(Cost val) { h = val; }
		void setGValue(Cost val) { g = val; }
		State getState() const { return stateRep; }
		Node* getParent() { return parent; }
		void setParent(Node* p) { parent = p; }
		bool onOpen() { return open; }
		void close() { open = false; }
		void reopen() { open = true; }

		Node(Cost g, Cost h, Cost derr, Cost eps, State treeNode, Node* parent)
			: g(g), h(h), derr(derr), eps(eps), stateRep(treeNode), parent(parent) {}
	};

	struct CompareNodes
	{
		bool operator()(const Node* n1, const Node* n2) const
		{
			// Tie break on heuristic
			if (n1->getFHatValue() == n2->getFHatValue())
				return n1->getFValue() > n2->getFValue();
			return n1->getFHatValue() > n2->getFHatValue();
		}
	};

	AStarFHat(D& domain) : domain(domain) {}

	~AStarFHat()
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
					}
				}
				return true;
			}
		}

		return false;
	}

	Node* aStar(Node* start, int lookahead, ResultContainer& res)
	{
		int expansions = 0;

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

		// Push start on open
		open.push(start);

		while (!open.empty() && (expansions < lookahead))
		{
			// Pop lowest fhat-value off open
			Node* node = open.top();
			open.pop();
			node->close();
			closed[node->getState().hash()].push_back(node);
			res.nodesExpanded++;
			expansions++;

			// Check if current node is goal
			if (domain.isGoal(node->getState()))
			{
				return node;
			}

			vector<State> children = domain.successors(node->getState());
			res.nodesGenerated += children.size();
			for (State child : children)
			{
				Node* childNode = new Node(node->getGValue() + domain.getEdgeCost(child),
					domain.heuristic(child), domain.distance(child), eps, child, node);
				// Duplicate detection
				if (!duplicateDetection(childNode))
				{
					open.push(childNode);
					openUclosed[child.hash()].push_back(childNode);
				}
				else
					delete childNode;
			}
		}

		return NULL;
	}

	ResultContainer search(int lookahead)
	{
		ResultContainer res;
		res.solutionCost = 0;
		res.nodesExpanded = 0;
		res.nodesGenerated = 0;
		res.solutionFound = false;

		// Get the start node
		Node* start = new Node(0, 0, domain.distance(domain.getStartState()), eps, domain.getStartState(), NULL);
		openUclosed[start->getState().hash()].push_back(start);

		while (1)
		{
			if (domain.isGoal(start->getState()))
			{
				// Solution found, stop timer

				// Calculate path cost and return solution
				calculateCost(start, res);

				return res;
			}

			Node* goalPrime = aStar(start, lookahead, res);

			if (open.empty() && goalPrime == NULL)
			{
				break;
			}

			if (goalPrime == NULL)
			{
				goalPrime = open.top();
				open.pop();
			}

			// Only move one step towards best on open
			while (goalPrime->getParent() != start)
				goalPrime = goalPrime->getParent();

			start = goalPrime;
		}
		return res;
	}

private:
	D& domain;
	priority_queue<Node*, vector<Node*>, CompareNodes> open;
	unordered_map<unsigned long, vector<Node*> > closed;
	unordered_map<unsigned long, vector<Node*> > openUclosed;

	void calculateCost(Node* solution, ResultContainer& res)
	{
		res.solutionFound = true;
		res.solutionCost = solution->getFValue();
	}
};