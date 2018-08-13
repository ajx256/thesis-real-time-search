#pragma once
#include <vector>
#include <set>
#include <functional>
#include <unordered_map>
#include "utility/DiscreteDistribution.h"
#include "utility/PriorityQueue.h"
#include "utility/ResultContainer.h"
#include "decisionAlgorithms/DecisionAlgorithm.h"
#include "decisionAlgorithms/KBestBackup.h"
#include "decisionAlgorithms/ScalarBackup.h"
#include "expansionAlgorithms/ExpansionAlgorithm.h"
#include "expansionAlgorithms/AStar.h"
#include "expansionAlgorithms/BreadthFirst.h"
#include "expansionAlgorithms/DepthFirst.h"
#include "expansionAlgorithms/Risk.h"
#include "learningAlgorithms/LearningAlgorithm.h"
#include "learningAlgorithms/Dijkstra.h"
#include "learningAlgorithms/Ignorance.h"

using namespace std;

template <class Domain>
class RealTimeSearch
{
public:
	typedef typename Domain::State State;
	typedef typename Domain::Cost Cost;
	typedef typename Domain::HashState Hash;

	struct Node
	{
		Cost g;
		Cost h;
		Cost d;
		Cost eps;
		Node* parent;
		State stateRep;
		set<int> owningTLAs;
		bool open;
		int delayCntr;
		DiscreteDistribution distribution;

	public:
		Cost getGValue() const { return g; }
		Cost getHValue() const { return h; }
		Cost getDValue() const { return d; }
		Cost getFValue() const { return g + h; }
		Cost getFHatValue() const { return g + h + (d * eps); }
		void setHValue(Cost val) { h = val; }
		void setGValue(Cost val) { g = val; }
		void setDValue(Cost val) { d = val; }
		State getState() const { return stateRep; }
		Node* getParent() { return parent; }
		void addOwningTLA(int tla) { owningTLAs.insert(tla); }
		set<int> getOwningTLAs() { return owningTLAs; }
		Cost getD() { return d; }
		Cost getHeuristicError() { return d * eps; }
		void setParent(Node* p) { parent = p; }
		bool onOpen() { return open; }
		void close() { open = false; }
		void reopen() { open = true; }
		void incDelayCntr() { delayCntr++; }
		int getDelayCntr() { return delayCntr; }

		Node(Cost g, Cost h, Cost d, Cost eps, State state, Node* parent, set<int> tlas)
			: g(g), h(h), d(d), eps(eps), stateRep(state), parent(parent)
		{
			open = true;
			delayCntr = 0;
			for (int tla : tlas)
			{
				owningTLAs.insert(tla);
			}
		}

		static bool compareNodesF(const Node* n1, const Node* n2)
		{
			// Tie break on heuristic
			if (n1->getFValue() == n2->getFValue())
				return n1->getGValue() < n2->getGValue();
			return n1->getFValue() > n2->getFValue();
		}

		static bool compareNodesFHat(const Node* n1, const Node* n2)
		{
			// Tie break on f-value
			if (n1->getFHatValue() == n2->getFHatValue())
				return n1->getFValue() > n2->getFValue();
			return n1->getFHatValue() > n2->getFHatValue();
		}

		static bool compareNodesH(const Node* n1, const Node* n2)
		{
			return n1->getHValue() > n2->getHValue();
		}
	};

	struct TopLevelAction
	{
	public:
		PriorityQueue<Node*> open;
		Cost expectedMinimumPathCost;
		Node* topLevelNode;
		vector<Node*> kBestNodes;
		DiscreteDistribution belief;

		TopLevelAction()
		{
			open.swapComparator(Node::compareNodesFHat);
		}

		TopLevelAction(const TopLevelAction& tla)
		{
			open = tla.open;
			expectedMinimumPathCost = tla.expectedMinimumPathCost;
			topLevelNode = tla.topLevelNode;
			kBestNodes = tla.kBestNodes;
			belief = tla.belief;
		}

		TopLevelAction& operator=(const TopLevelAction& rhs)
		{
			if (&rhs == this)
				return *this;
			open = rhs.open;
			expectedMinimumPathCost = rhs.expectedMinimumPathCost;
			topLevelNode = rhs.topLevelNode;
			kBestNodes = rhs.kBestNodes;
			belief = rhs.belief;
			return *this;
		}
	};

	RealTimeSearch(Domain& domain, string expansionModule, string learningModule,
		string decisionModule, double lookahead, double k = 1, string belief = "normal")
		: domain(domain), lookahead(lookahead)
	{
		setEpsilon(expansionModule, lookahead);

		if (expansionModule == "a-star")
		{
			expansionAlgo = new AStar<Domain, Node, TopLevelAction>(domain, lookahead, "f", eps);
		}
		else if (expansionModule == "f-hat")
		{
			expansionAlgo = new AStar<Domain, Node, TopLevelAction>(domain, lookahead, "fhat", eps);
		}
		else if (expansionModule == "dfs")
		{
			expansionAlgo = new DepthFirst<Domain, Node, TopLevelAction>(domain, lookahead, eps);
		}
		else if (expansionModule == "bfs")
		{
			expansionAlgo = new BreadthFirst<Domain, Node, TopLevelAction>(domain, lookahead, eps);
		}
		else if (expansionModule == "risk")
		{
			expansionAlgo = new Risk<Domain, Node, TopLevelAction>(domain, lookahead, 1, eps);
		}

		if (learningModule == "none")
		{
			learningAlgo = new Ignorance<Domain, Node, TopLevelAction>;
		}
		else if (learningModule == "learn")
		{
			learningAlgo = new Dijkstra<Domain, Node, TopLevelAction>(domain);
		}

		if (decisionModule == "minimin")
		{
			decisionAlgo = new ScalarBackup<Domain, Node, TopLevelAction>("minimin");
		}
		else if (decisionModule == "bellman")
		{
			decisionAlgo = new ScalarBackup<Domain, Node, TopLevelAction>("bellman");
		}
		else if (decisionModule == "k-best")
		{
			decisionAlgo = new KBestBackup<Domain, Node, TopLevelAction>(domain, k, belief, lookahead, eps);
		}
	}

	~RealTimeSearch()
	{
		clean();
	}

	ResultContainer search()
	{
		ResultContainer res;

		// Get the start node
		set<int> empty;
		Node* start = new Node(0, 0, domain.distance(domain.getStartState()), 
			eps, domain.getStartState(), NULL, empty);

		while (1)
		{
			// Check if a goal has been reached
			if (domain.isGoal(start->getState()))
			{
				// Calculate path cost and return solution
				calculateCost(start, res);

				return res;
			}

			restartLists(start);

			// Exploration Phase

			// First, generate the top-level actions
			generateTopLevelActions(start, res);

			// Expand some nodes until expnasion limit
			expansionAlgo->expand(open, closed, tlas, duplicateDetection, res);

			// Check if this is a dead end
			if (open.empty())
			{
				break;
			}

			//  Learning Phase
			learningAlgo->learn(open, closed);

			// Decision-making Phase
			start = decisionAlgo->backup(open, tlas, start);
		}

		return res;
	}

	ResultContainer lastIncrementalDecision()
	{
		ResultContainer res;

		set<int> empty;
		// Get the start node
		Node* start = new Node(0, 0, domain.distance(domain.getStartState()),
			eps, domain.getStartState(), NULL, empty);

		// Check if a goal has been reached
		if (domain.isGoal(start->getState()))
		{
			// Calculate path cost and return solution
			calculateCost(start, res);

			return res;
		}

		restartLists(start);

		// Exploration Phase

		// First, generate the top-level actions
		generateTopLevelActions(start, res);

		// Expand some nodes until expnasion limit
		expansionAlgo->expand(open, closed, tlas, duplicateDetection, res);

		//  Learning Phase
		learningAlgo->learn(open, closed);

		// Decision-making Phase
		start = decisionAlgo->backup(open, tlas, start);

		// Empty OPEN and CLOSED
		open.clear();

		// delete all of the nodes from the last expansion phase
		for (typename unordered_map<State, Node*, Hash>::iterator it = closed.begin(); it != closed.end(); it++)
			if (it->second != start)
				delete it->second;

		closed.clear();

		open.push(start);
		closed[start->getState()] = start;

		expansionAlgo->incrementLookahead();

		// Solve the search optimally
		expansionAlgo->expand(open, closed, tlas, duplicateDetection, res);

		open.swapComparator(Node::compareNodesF);
		start = open.top();

		if (domain.isGoal(start->getState()))
		{
			// Calculate path cost and return solution
			calculateCost(start, res);

			return res;
		}
	}

private:
	static bool duplicateDetection(State s, Cost g, Cost h, Cost d, Node* parent, set<int> tlas, unordered_map<State, Node*, Hash>& closed)
	{
		// Check if this state exists 
		typename unordered_map<State, Node*, Hash>::iterator it = closed.find(s);

		if (it != closed.end())
		{
			// This state has been generated before, check if its node is on OPEN
			if (it->second->onOpen())
			{
				// This node is on OPEN, keep the better g-value
				if (g < it->second->getGValue())
				{
					it->second->setGValue(g);
					it->second->setParent(parent);
				}
			}
			else
			{
				// This node is on CLOSED, compare the f-values. If this new f-value is better, reset g, h, and d. 
				// Then reopen the node.
				/*
				if (g + h < it->second->getFValue())
				{
					it->second->setGValue(g);
					it->second->setParent(parent);
					it->second->setHValue(h);
					it->second->setDValue(d);
					it->second->reOpen();
					open.push(it->second);
				}
				*/
			}

			// Keep track of the TLA this state was generated under
			//it->second->addOwningTLA(tla);

			return true;
		}

		return false;
	}

	void generateTopLevelActions(Node* start, ResultContainer& res)
	{
		// The first node to be expanded in any problem is the start node
		// Doing so yields the top level actions
		start->close();
		closed[start->getState()] = start;
		res.nodesExpanded++;

		vector<State> children = domain.successors(start->getState());
		res.nodesGenerated += children.size();
		for (State child : children)
		{
			set<int> tlaSet;
			tlaSet.insert(tlas.size());
			Node* childNode = new Node(start->getGValue() + domain.getEdgeCost(child),
				domain.heuristic(child), domain.distance(child), eps,
				child, start, tlaSet);

			// No top level action will ever be a duplicate, so no need to check.
			// Make a new top level action and push this node onto its open
			TopLevelAction tla;
			tla.topLevelNode = childNode;

			childNode->distribution = DiscreteDistribution(100, childNode->getFValue(), childNode->getFHatValue(), 
				childNode->getD(), eps);

			tla.expectedMinimumPathCost = childNode->distribution.expectedCost();

			// Push this node onto open and closed
			closed[child] = childNode;
			open.push(childNode);
			tla.open.push(childNode);

			// Add this top level action to the list
			tlas.push_back(tla);
		}
	}

	void restartLists(Node* start)
	{
		// clear the TLA list
		tlas.clear();

		// Empty OPEN and CLOSED
		open.clear();

		// delete all of the nodes from the last expansion phase
		for (typename unordered_map<State, Node*, Hash>::iterator it = closed.begin(); it != closed.end(); it++)
			if (it->second != start)
				delete it->second;

		closed.clear();
	}

	void clean()
	{
		// clear the TLA list
		tlas.clear();

		// Empty OPEN and CLOSED
		open.clear();

		// delete all of the nodes from the last expansion phase
		for (typename unordered_map<State, Node*, Hash>::iterator it = closed.begin(); it != closed.end(); it++)
			delete it->second;

		closed.clear();

		delete expansionAlgo;
		delete learningAlgo;
		delete decisionAlgo;
	}

	void calculateCost(Node* solution, ResultContainer& res)
	{
		res.solutionFound = true;
		res.solutionCost = solution->getFValue();
	}

	void setEpsilon(string expansionModule, int la)
	{
		if (expansionModule == "dfs")
		{
			switch (la)
			{
			case 3:
				eps = 0.27;
				break;
			case 7:
				eps = 0.24;
				break;
			case 9:
				eps = 0.23;
				break;
			case 10:
				eps = 0.225;
				break;
			case 14:
				eps = 0.22;
				break;
			default:
				break;
			}
		}
		else
		{
			switch (la)
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
	}

protected:
	Domain& domain;
	ExpansionAlgorithm<Domain, Node, TopLevelAction>* expansionAlgo;
	LearningAlgorithm<Domain, Node, TopLevelAction>* learningAlgo;
	DecisionAlgorithm<Domain, Node, TopLevelAction>* decisionAlgo;
	PriorityQueue<Node*> open;
	unordered_map<State, Node*, Hash> closed;
	vector<TopLevelAction> tlas;

	Cost eps;
	double lookahead;
};