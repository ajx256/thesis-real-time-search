#pragma once
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <string>
#include <algorithm>
#include "../utility/PriorityQueue.h"
#include "../utility/DiscreteDistribution.h"
#include "../utility/ResultContainer.h"

using namespace std;

# define M_PI 3.14159265358979323846  /* pi */

template <class D>
struct RiskNancyBackup
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
		int delayCntr;
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
		void incDelayCntr() { delayCntr++; }
		int getDelayCntr() { return delayCntr; }

		Node(Cost g, Cost h, Cost derr, Cost eps, State treeNode, Node* parent, int tla)
			: g(g), h(h), derr(derr), eps(eps), stateRep(treeNode), parent(parent), owningTLA(tla)
		{
			delayCntr = 0;
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
		PriorityQueue<Node*> topLevelOpen;
		Cost expectedMinimumPathCost;
		Node* topLevelNode;
		vector<Node*> kBestNodes;
		DiscreteDistribution belief;

		TopLevelAction()
		{
			topLevelOpen.swapComparator(Node::compareNodesFHat);
		}

		TopLevelAction(const TopLevelAction& tla)
		{
			topLevelOpen = tla.topLevelOpen;
			expectedMinimumPathCost = tla.expectedMinimumPathCost;
			topLevelNode = tla.topLevelNode;
			kBestNodes = tla.kBestNodes;
			belief = tla.belief;
		}

		TopLevelAction& operator=(const TopLevelAction& rhs)
		{
			if (&rhs == this)
				return *this;
			topLevelOpen = rhs.topLevelOpen;
			expectedMinimumPathCost = rhs.expectedMinimumPathCost;
			topLevelNode = rhs.topLevelNode;
			kBestNodes = rhs.kBestNodes;
			belief = rhs.belief;
			return *this;
		}
	};

	RiskNancyBackup(D& domain, int lookahead, int expansionAllocation) : domain(domain), lookahead(lookahead), expansionsPerIteration(expansionAllocation) 
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

		open.swapComparator(Node::compareNodesFHat);
	}

	~RiskNancyBackup()
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

	ResultContainer search()
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
			generateTopLevelActions(start, res);

			// Expand some nodes until expnasion limit
			explore(res);

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

	//---------------------------------------- Functions for the EXPANSION PHASE -------------------------------------------------------------

	void generateTopLevelActions(Node* start, ResultContainer& res)
	{
		// The first node to be expanded in any problem is the start node
		// Doing so yields the top level actions
		start->close();
		closed[start->getState().hash()].push_back(start);
		res.nodesExpanded++;

		// Expand the start node to get the top level states (under the TLAs)
		vector<State> children = domain.successors(start->getState());
		res.nodesGenerated += children.size();
		for (State child : children)
		{
			Node* childNode = new Node(start->getGValue() + domain.getEdgeCost(child),
				domain.heuristic(child), domain.distance(child), eps, child, start, topLevelActions.size());

			// No top level action will ever be a duplicate, so no need to check.
			// Make a new top level action and push this node onto its open
			TopLevelAction tla;
			tla.topLevelOpen.push(childNode);
			tla.topLevelNode = childNode;

			// Initialize the belief distribution at this top level node and assign that as the expected path cost through this TLA
			childNode->distribution = DiscreteDistribution(100, childNode->getFValue(), childNode->getFHatValue(), childNode->getD(), eps);
			tla.expectedMinimumPathCost = childNode->distribution.expectedCost();

			// Push this node onto open
			openUclosed[childNode->getState().hash()].push_back(childNode);
			open.push(childNode);

			// Add this top level action to the list
			topLevelActions.push_back(tla);
		}
	}

	void explore(ResultContainer& res)
	{
		// This starts at 1, because we had to expand start to get the top level actions
		int expansions = 1;

		// TODO: IMPLEMENT RISK BASED EXPANSION
		while (expansions < lookahead && !open.empty())
		{
			// At the beginning of each expansion we are going to update our beliefs at every TLA.
			// Why? Because this could be the first expansion in the phase.
			// If it isn't? Wouldn't we only need to update the TLA of the node we expanded last iteration?
			// In a tree, yes. But in a graph, this could have placed nodes onto the open lists of other TLAs.
			// Therefore, the beliefs of all TLAs should be updated before every expansion.
			kBestDecision();
			
			// Simulate expansion of best node under each TLA
			int chosenTLAIndex = simulateExpansion();

			// Expand under the TLA which holds the lowest risk
			Node* chosenNode = topLevelActions[chosenTLAIndex].topLevelOpen.top();

			// Remove the chosen node from open
			topLevelActions[chosenTLAIndex].topLevelOpen.pop();
			open.remove(chosenNode);

			// Add the chosen node to closed
			chosenNode->close();
			closed[chosenNode->getState().hash()].push_back(chosenNode);

			// Book keeping for expansion count
			res.nodesExpanded++;
			expansions++;

			// Add this node to the expansion delay window
			domain.pushDelayWindow(chosenNode->getDelayCntr());

			// Increment the delay counts of every other node on open...
			for (Node* n : open)
			{
				n->incDelayCntr();
			}

			// Check if current node is goal. If it is, then the expansion phase is over, time to move.
			if (domain.isGoal(chosenNode->getState()))
			{
				return;
			}

			// Generate the successors of the chosen node
			vector<State> children = domain.successors(chosenNode->getState());

			// Book keeping for number of nodes generated
			res.nodesGenerated += children.size();
			
			// Iterate over the successor states
			for (State child : children)
			{
				// Create a node for this state
				Node* childNode = new Node(chosenNode->getGValue() + domain.getEdgeCost(child),
					domain.heuristic(child), domain.distance(child), eps, child, chosenNode, chosenTLAIndex);

				// Duplicate detection performed
				if (!duplicateDetection(childNode))
				{
					// If this state hasn't yet been reached, add this node open 
					open.push(childNode);
					openUclosed[child.hash()].push_back(childNode);
					// Add to open of owning TLA
					topLevelActions[chosenTLAIndex].topLevelOpen.push(childNode);
				}
				else
				{
					// If this state has already been visited, delete the node, one already exists with minimal g-value
					delete childNode;
				}
			}
		}
	}

	int simulateExpansion()
	{
		int minimalRiskTLA = 0;
		double minimalRisk = 0;

		// Start by identifying alpha: the TLA with lowest expected cost
		int alphaTLA = 0;
		double alphaExpectedCost = topLevelActions[0].belief.expectedCost();
		for (int i = 1; i < topLevelActions.size(); i++)
		{
			if (topLevelActions[i].belief.expectedCost() < alphaExpectedCost)
			{
				alphaExpectedCost = topLevelActions[i].belief.expectedCost();
				alphaTLA = i;
			}
		}

		// Iterate over the top level actions
		for (int i = 0; i < topLevelActions.size(); i++)
		{
			// Simulate how expanding this TLA's best node would affect its belief
			// Belief of TLA is squished as a result of search. Mean stays the same, but variance is decreased by a factor based on expansion delay.
			double ds = expansionsPerIteration / domain.averageDelayWindow();
			double dy = topLevelActions[i].topLevelOpen.top()->getD();
			double squishFactor = min(1.0, (ds / dy));

			// Now squish the simulated belief by factor
			vector<TopLevelAction> squishedTopLevelActions = topLevelActions;
			squishedTopLevelActions[i].belief.squish(squishFactor);

			// Calculate the risk associated with expanding that node (by using the simulated belief as alpha in risk analysis)
			double riskCalculation = riskAnalysis(alphaTLA, squishedTopLevelActions);
			
			// If this is the first TLA risk has been calculated for, it by default minimizes risk...
			if (i == 0)
				minimalRisk = riskCalculation;
			else if (i > 0 && riskCalculation < minimalRisk)
			{
				// Otherwise the TLA with the lower risk replaces the current lowest
				minimalRisk = riskCalculation;
				minimalRiskTLA = i;
			}
		}

		return minimalRiskTLA;
	}

	double riskAnalysis(int alphaIndex, vector<TopLevelAction>& squishedTopLevelActions)
	{
		double risk = 0;

		// Perform numerical integration to calculate risk associated with taking alpha as the expansion
		for (auto alpha : squishedTopLevelActions[alphaIndex].belief)
		{
			for (int tla = 0; tla < squishedTopLevelActions.size(); tla++)
			{
				// Don't integrate over alpha as a beta action
				if (tla == alphaIndex)
					continue;

				// Integrate over values in beta belief
				for (auto beta : squishedTopLevelActions[tla].belief)
				{
					// Only use beta costs less than alpha cost in risk analysis
					if (beta.cost < alpha.cost)
					{
						// Calculate the risk
						double value = alpha.probability * beta.probability * (alpha.cost - beta.cost);
						risk += value;
					}
					else
						break;
				}
			}
		}

		return risk;
	}

	//---------------------------------------- Functions for the BACKUP PHASE ------------------------------------------------------------------

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
			tla.belief = tla.kBestNodes[0]->distribution;
		}
		else
		{
			// If nothing was expanded under this TLA, use the expected value of the TLA
			tla.expectedMinimumPathCost = tla.topLevelNode->distribution.expectedCost();
			tla.belief = tla.topLevelNode->distribution;
		}
	}

	void kBestDecision()
	{
		// The K-Best decision assumes that the only nodes within the subtrees of the TLAs are the k-best frontier nodes
		// on their opened lists. Find them.
		for (TopLevelAction& tla : topLevelActions)
		{
			tla.kBestNodes.clear();

			int i = 0;
			// Add to the best k nodes while i < k and non-selected nodes exist on the frontier
			while (i < k && !tla.topLevelOpen.empty())
			{
				Node* best = tla.topLevelOpen.top();
				tla.topLevelOpen.pop();

				// Make this node's PDF a discrete distribution...
				best->distribution = DiscreteDistribution(100, best->getFValue(), best->getFHatValue(), best->getD(), eps);

				tla.kBestNodes.push_back(best);
				i++;
			}

			// Now put the nodes back in the top level open list
			for (Node* n : tla.kBestNodes)
			{
				tla.topLevelOpen.push(n);
			}

			// Now that k-best are selected, perform Cserna backup
			csernaBackup(tla);
		}
	}

private:
	D & domain;
	vector<TopLevelAction> topLevelActions;
	PriorityQueue<Node*> open;
	unordered_map<unsigned long, vector<Node*> > closed;
	unordered_map<unsigned long, vector<Node*> > openUclosed;
	int k = 1;
	double expansionsPerIteration;
	int lookahead;

	void calculateCost(Node* solution, ResultContainer& res)
	{
		res.solutionFound = true;
		res.solutionCost = solution->getFValue();
	}
};