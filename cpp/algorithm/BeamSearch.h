#pragma once
#include <iostream>
#include <queue>
#include <vector>
#include <unordered_map>
#include "../utility/ResultContainer.h"

using namespace std;

template <class D>
struct BeamSearch
{
	typedef typename D::State State;
	typedef typename D::Cost Cost;

	struct Node
	{
		Cost g;
		Node* parent;
		State stateRep;
		bool open;

	public:
		Cost getGValue() const { return g; }
		void setGValue(Cost val) { g = val; }
		State getState() const { return stateRep; }
		Node* getParent() { return parent; }
		void setParent(Node* p) { parent = p; }
		bool onOpen() { return open; }
		void close() { open = false; }
		void reopen() { open = true; }

		Node(Cost g, State treeNode, Node* parent)
			: g(g), stateRep(treeNode), parent(parent) {}
	};

	struct CompareNodes
	{
		bool operator()(const Node* n1, const Node* n2) const
		{
			return n1->getGValue() > n2->getGValue();
		}
	};

	struct CompareNodesGreatest
	{
		bool operator()(const Node* n1, const Node* n2) const
		{
			return n1->getGValue() < n2->getGValue();
		}
	};

	BeamSearch(D& domain, int beam) : domain(domain), beam(beam) {}

	~BeamSearch()
	{
		// delete all of the nodes from the last expansion phase
		for (typename unordered_map<unsigned long, vector<Node*> >::iterator it = openUclosed.begin(); it != openUclosed.end(); it++)
			for (typename vector<Node*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
				delete *it2;

		openUclosed.clear();
	}

	priority_queue<Node*, vector<Node*>, CompareNodes> swapComparatorMaxMin(priority_queue<Node*, vector<Node*>, CompareNodesGreatest> layer)
	{
		priority_queue<Node*, vector<Node*>, CompareNodes> newLayer;

		while (!layer.empty())
		{
			newLayer.push(layer.top());
			layer.pop();
		}

		return newLayer;
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

	Node* breadthFirstSearch(priority_queue<Node*, vector<Node*>, CompareNodes> layer, int lookahead, int expansions, ResultContainer& res)
	{
		// keep track of the best node at the current layer
		Node* bestAtLayer = layer.top();

		priority_queue<Node*, vector<Node*>, CompareNodesGreatest> nextLayerMax;

		while (!layer.empty() && expansions < lookahead)
		{
			// Get the next node to expand
			Node* node = layer.top();
			layer.pop();
			node->close();
			closed[node->getState().hash()].push_back(node);
			res.nodesExpanded++;
			expansions++;

			// Check if the current node is the goal
			if (domain.isGoal(node->getState()))
			{
				return node;
			}

			// Expand this node
			vector<State> children = domain.successors(node->getState());
			res.nodesGenerated += children.size();

			for (State child : children)
			{
				// Push this child onto open
				Node* childNode = new Node(node->getGValue() + domain.getEdgeCost(child), child, node);
				// Duplicate detection
				if (!duplicateDetection(childNode))
				{
					// Determine if this node fits within the beam
					if (nextLayerMax.size() < beam)
					{
						nextLayerMax.push(childNode);
						openUclosed[child.hash()].push_back(childNode);
					}
					else
					{
						// The beam for this layer is full, check if this is better than the worst in the beam
						if (nextLayerMax.top()->getGValue() > childNode->getGValue())
						{
							// This node is better than the worst in the beam, exchange them
							nextLayerMax.pop();
							nextLayerMax.push(childNode);
							openUclosed[child.hash()].push_back(childNode);
						}
						else
							delete childNode;
					}
				}
				else
					delete childNode;
			}
		}
		
		// Now that this layer is fully expanded, and the nodes within the beam are set in stone, swap to a min queue comparator
		priority_queue<Node*, vector<Node*>, CompareNodes> nextLayerMin = swapComparatorMaxMin(nextLayerMax);
		Node* bestDeeper = NULL;
		// If this layer is complete and still haven't hit expansion limit, go to next layer
		if (layer.empty() && expansions < lookahead && !nextLayerMin.empty())
		{
			bestDeeper = breadthFirstSearch(nextLayerMin, lookahead, expansions, res);
		}

		// If a result came up from the next layer, use that
		if (bestDeeper != NULL)
			return bestDeeper;
		else
		{
			// Return the best node generated, if there are any.
			// Otherwise return the best node that was expanded in this layer.
			if (!nextLayerMin.empty())
				return nextLayerMin.top();
			else
				return bestAtLayer;
		}
	}

	ResultContainer search(int lookahead)
	{
		ResultContainer res;
		res.solutionCost = 0;
		res.nodesExpanded = 0;
		res.nodesGenerated = 0;
		res.solutionFound = false;

		// Get the initial state/node
		Node* start = new Node(0, domain.getStartState(), NULL);
		openUclosed[start->getState().hash()].push_back(start);
		priority_queue<Node*, vector<Node*>, CompareNodes> firstLayer;
		firstLayer.push(start);

		while (1)
		{
			// Empty CLOSED
			closed.clear();

			// delete all of the nodes from the last expansion phase
			for (typename unordered_map<unsigned long, vector<Node*> >::iterator it = openUclosed.begin(); it != openUclosed.end(); it++)
				for (typename vector<Node*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
					if (*it2 != start)
						delete *it2;

			openUclosed.clear();
			openUclosed[start->getState().hash()].push_back(start);

			if (domain.isGoal(start->getState()))
			{
				// Solution found, stop timer

				// Calculate path cost and return solution
				calculateCost(start, res);

				return res;
			}

			Node* goalPrime = breadthFirstSearch(firstLayer, lookahead, 0, res);

			if (goalPrime == NULL || goalPrime == start)
			{
				break;
			}

			// Only move one step towards best node at the most recent beam
			while (goalPrime->getParent() != start)
				goalPrime = goalPrime->getParent();

			start = goalPrime;

			firstLayer.pop();
			firstLayer.push(start);
		}

		return res;
	}

private:
	D& domain;
	int beam;
	unordered_map<unsigned long, vector<Node*> > closed;
	unordered_map<unsigned long, vector<Node*> > openUclosed;

	void calculateCost(Node* solution, ResultContainer& res)
	{
		res.solutionFound = true;
		res.solutionCost = solution->getGValue();
	}
};