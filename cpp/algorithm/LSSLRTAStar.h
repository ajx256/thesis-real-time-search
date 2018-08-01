#pragma once
#include <iostream>
#include <queue>
#include <unordered_map>

using namespace std;

template <class D>
struct LSSLRTAStar
{
	typedef typename D::State State;

	struct Node
	{
		double g;
		double h;
		double horig;
		Node* parent;
		State stateRep;
		bool open;

	public:
		double getGValue() const { return g; }
		double getHValue() const { return h; }
		double getFValue() const { return g + h; }
		void setGValue(double val) { g = val; }
		void setHValue(double val) { h = val; }
		State getState() const { return stateRep; }
		Node* getParent() { return parent; }
		bool onOpen() { return open; }
		void close() { open = false; }
		void reopen() { open = true; }

		Node(double g, State treeNode, Node* parent)
			: g(g), parent(parent)
		{
			stateRep = treeNode;
			open = true;
			h = stateRep->h();
			horig = h;
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

	struct CompareNodesGreedy
	{
		bool operator()(const Node* n1, const Node* n2) const
		{
			return n1->getHValue() > n2->getHValue();
		}
	};

	struct resultContainer
	{
	public:
		Node* solution;
		int nodesGen;
		int nodesExpand;
		double newBound;
	};

	LSSLRTAStar(D& domain, int lookahead) : domain(domain), lookahead(lookahead)
	{
		switch (lookahead)
		{
		case 3:
			eps = 0.295;
			break;
		case 6:
			eps = 0.27;
			break;
		case 10:
			eps = 0.26;
			break;
		case 30:
			eps = 0.23;
			break;
		case 100:
			eps = 0.225;
			break;
		case 1000:
			eps = 0.223;
			break;
		default:
			break;
		}
	}

	~LSSLRTAStar()
	{
		tree.clear();
	}

	void dijkstra()
	{
		// Initialize all h(n) in closed to infinity
		for (typename map<unsigned long, Node*>::iterator it = closed.begin(); it != closed.end(); it++)
			it->second->setHValue(numeric_limits<float>::infinity());

		while (!closed.empty())
		{
			// Remove best value from open
			Node* s = open.top();
			s->close();
			open.pop();
			// Delete it from closed
			closed.erase(s->getState()->hash());

			if (s->getParent())
			{
				Node* p = s->getParent();
				if (closed.find(p->getState()->hash()) != closed.end() && p->getHValue() > (s->getState()->getEdgeCost() + s->getHValue()))
				{
					// Update the parent's heuristic
					p->setHValue(s->getState()->getEdgeCost() + s->getHValue());
					if (!p->onOpen())
					{
						p->reopen();
						open.push(p);
					}
				}
			}
		}
	}

	resultContainer aStar(Node* start)
	{
		resultContainer res;
		res.solution = NULL;
		res.nodesGen = 0;
		res.nodesExpand = 0;

		int expansions = 0;

		// Empty OPEN and CLOSED
		while (!open.empty())
			open.pop();

		closed.clear();

		// Push start on open and closed
		open.push(start);
		tree[start->getState()->hash()] = start;

		while (!open.empty() && (expansions < lookahead))
		{
			// Pop lowest g-value off open
			Node* node = open.top();
			open.pop();
			node->close();
			closed[node->getState()->hash()] = node;
			res.nodesExpand++;

			expansions++;

			// Check if current node is goal
			if (node->getState()->isGoal())
			{
				res.solution = node;
				return res;
			}

			vector<State> children = node->getState()->generateSuccessors();
			res.nodesGen = res.nodesGen + children.size();
			for (State child : children)
			{
				// No need for duplicate detection in tree world
				Node* childNode = new Node(node->getGValue() + child->getEdgeCost(), child, node);
				open.push(childNode);
				tree[child->hash()] = childNode;
			}
		}

		return res;
	}

	resultContainer search(Node* start)
	{
		resultContainer finalRes;
		finalRes.solution = NULL;
		finalRes.nodesExpand = 0;
		finalRes.nodesGen = 0;

		resultContainer tmpRes;
		tmpRes.solution = NULL;
		tmpRes.nodesExpand = 0;
		tmpRes.nodesGen = 0;

		while (1)
		{
			tmpRes = aStar(start);
			finalRes.solution = tmpRes.solution;
			finalRes.nodesExpand += tmpRes.nodesExpand;
			finalRes.nodesGen += tmpRes.nodesGen;
			if (finalRes.solution != NULL)
				return finalRes;
			if (open.empty())
			{
				break;
			}
			Node* goalPrime = open.top();

			// Only move one step towards best on open
			while (goalPrime->getParent() != start)
				goalPrime = goalPrime->getParent();

			// Learning step
			dijkstra();

			start = goalPrime;
		}
		return finalRes;
	}

	private:
		priority_queue<Node*, vector<Node*>, CompareNodes> open;
		unordered_map<unsigned long, Node*> closed;
		unordered_map<unsigned long, Node*> tree;
		D& domain;
		int lookahead;
};