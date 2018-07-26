#pragma once
#include <iostream>
#include <queue>
#include <unordered_map>
#include <cmath>
#include "../utility/DiscreteDistribution.h"
#include "../utility/ResultContainer.h"

using namespace std;

template <class D>
struct Nancy
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
		Cost getDError() { return derr * eps; }
		Cost getD() { return derr; }
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
		vector<Node*> topLevelOpen;
		double mean;
		double var;
		Cost expectedMinimumPathCost;
		Node* topLevelNode;
	};

	Nancy(D& domain) : domain(domain) {}

	~Nancy()
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

	void generateTopLevelActions(Node* start, ResultContainer& res)
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
				domain.heuristic(child), domain.distance(child), eps, child, start);
			// No top level action will ever be a duplicate, so no need to check.
			// Make a new top level action and push this node onto its open
			TopLevelAction tla;
			tla.topLevelOpen.push_back(childNode);
			tla.topLevelNode = childNode;
			tla.mean = start->getFHatValue();
			// Standard deviation is d(n) * (1/(branching factor + 1))
			tla.var = pow((domain.distance(child) / (domain.getBranchingFactor() + 1)), 2);
			// Push this node onto open
			open.push(childNode);
			openUclosed[childNode->getState().hash()].push_back(childNode);
			// Add this top level action to the list
			topLevelActions.push_back(tla);
		}
	}


	// Test this for known distributions
	double phi(double x)
	{
		return (1.0 / 2.0) * erf((x / sqrt(2.0)));
	}

	void csernaBackup(TopLevelAction& tla)
	{
		// We assume in k-best that only the k-best nodes matter.
		if (!tla.topLevelOpen.empty())
		{
			// Perform Cserna Backups on k-best nodes
			while (tla.topLevelOpen.size() > 1)
			{
				// Take the first two and do a Cserna backup...
				tla.topLevelOpen[0]->distribution = tla.topLevelOpen[0]->distribution * tla.topLevelOpen[1]->distribution;
				// Remove the other used in the backup
				tla.topLevelOpen.erase(remove(tla.topLevelOpen.begin(), tla.topLevelOpen.end(), tla.topLevelOpen[1]), tla.topLevelOpen.end());
			}

			// Get the expected value of the resulting Cserna Distribution
			tla.expectedMinimumPathCost = tla.topLevelOpen[0]->distribution.expectedCost();
		}
		else
		{
			// If nothing was expanded under this TLA, use the expected value of the TLA
			tla.expectedMinimumPathCost = tla.topLevelNode->distribution.expectedCost();
		}
	}

	void csernaDecision()
	{
		// The K-Best decision assumes that the only nodes within the subtrees of the TLAs are the k-best frontier nodes
		// on their opened lists. Find them.
		for (TopLevelAction& tla : topLevelActions)
		{
			// Generate the distributions of the frontier nodes
			for(Node* node : tla.topLevelOpen)
			{
				// Make this node's PDF a discrete distribution...
				node->distribution = DiscreteDistribution(100, node->getFHatValue(), node->getD() / (domain.getBranchingFactor() + 1));
			}

			// Perform Cserna backup
			csernaBackup(tla);
		}
	}

	void explore(int lookahead, ResultContainer& res)
	{
		// This starts at 1, because we had to expand start to get the top level actions
		int expansions = 1;

		// Expand until the limit
		while (!open.empty() && (expansions < lookahead))
		{
			// If the only nodes left on open are goal nodes, we should stop expanding...
			bool allGoalsOpen1 = true;
			bool allGoalsOpen2 = true;

			for (Node* n : topLevelActions[0].topLevelOpen)
			{
				if (!domain.isGoal(n->getState()))
				{
					allGoalsOpen1 = false;
					break;
				}
			}
			for (Node* n : topLevelActions[1].topLevelOpen)
			{
				if (!domain.isGoal(n->getState()))
				{
					allGoalsOpen2 = false;
					break;
				}
			}

			if (allGoalsOpen1 && allGoalsOpen2)
			{
				return;
			}

			// The goal is to expand that which maximizes the change in expected confidence.
			// Confidence is the probability that a TLA is at least as good as any other TLA.
			// In this case, that a gaussian will yield a better value than another gaussian.
			// This can be computed by 1 - phi(-mu/sigma).
			// Where phi = 1/2[1 + erf(x/sqrt(2))]
			// s.t. mu = tla1.mean - tla2.mean
			// and sigma = tla1.var + tla2.var

			// Thus a change in confidence is the confidence computed with a TLA's current mean and var against another,
			// minus the confidence computed with the TLA's mean and var (with d(n) - 1) against another.

			// This is performed between all TLA's, and the node chosen for expansion is such that the difference is maximized.

			// TODO: Make this pick between more than two top level actions

			// TOP LEVEL ACTIONS INHERIT THE MEAN AND VAR OF THEIR BEST SUCCESSOR, NOT THE NODE JUST EXPANDED!
			// BELIEF IS CHANGED by change in variance, fhat will remain the same under a simulated expansion.
			// Done by decreasing d by one.

			// ONLY CONSIDERING LOWEST FHAT NODE MAKES THE ASSUMPTION THAT NO OTHER NODE ON FRONTIER COULD LEAD TO A BETTER CHILD,
			// HOWEVER, A NODE WITH A HIGHER FHAT AND A HIGHER VARIANCE COULD POTENTIALLY YIELD SUCH A RESULT

			sort(topLevelActions[0].topLevelOpen.begin(), topLevelActions[0].topLevelOpen.end(), CompareNodes());
			sort(topLevelActions[1].topLevelOpen.begin(), topLevelActions[1].topLevelOpen.end(), CompareNodes());

			double mu;
			double sigma;

			double confidenceBeforeUpdate;
			double confidenceAfterUpdate;

			double expectedChange1 = 0.0;
			double expectedChange2 = 0.0;

			if (!domain.isGoal(topLevelActions[0].topLevelOpen.back()->getState()))
			{
				// Check how the best node under TLA1 impacts confidence
				mu = topLevelActions[0].mean - topLevelActions[1].mean;
				sigma = sqrt(topLevelActions[0].var + topLevelActions[1].var);
				confidenceBeforeUpdate = 1.0 - phi((-mu) / sigma);

				// The change of confidence is done with the same mean, but decreasing the distance used in variance caluclation by 1
				sigma = sqrt(pow(((domain.distance(topLevelActions[0].topLevelOpen.back()->getState()) - 1) / (domain.getBranchingFactor() + 1)), 2) + topLevelActions[1].var);
				confidenceAfterUpdate = 1.0 - phi((-mu) / sigma);

				expectedChange1 = abs(confidenceBeforeUpdate - confidenceAfterUpdate);
			}
			if (!domain.isGoal(topLevelActions[1].topLevelOpen.back()->getState()))
			{
				// Check how the best node under TLA2 impacts confidence
				mu = topLevelActions[1].mean - topLevelActions[0].mean;
				sigma = sqrt(topLevelActions[1].var + topLevelActions[0].var);
				confidenceBeforeUpdate = 1.0 - phi((-mu) / sigma);


				// The change of confidence is done with the same mean, but decreasing the distance used in variance caluclation by 1
				sigma = sqrt(pow(((domain.distance(topLevelActions[1].topLevelOpen.back()->getState()) - 1) / (domain.getBranchingFactor() + 1)), 2) + topLevelActions[0].var);
				confidenceAfterUpdate = 1.0 - phi((-mu) / sigma);

				expectedChange2 = abs(confidenceBeforeUpdate - confidenceAfterUpdate);
			}
			if (expectedChange1 > expectedChange2)
			{
				// Expand under TLA1
				Node* node = topLevelActions[0].topLevelOpen.back();

				// Also remove this node from open...
				priority_queue<Node*, vector<Node*>, CompareNodes> tmpOpen;
				while (!open.empty())
				{
					if (open.top()->getState() != node->getState())
					{
						tmpOpen.push(open.top());
					}
					open.pop();
				}
				open = tmpOpen;

				node->close();
				closed[node->getState().hash()].push_back(node);
				res.nodesExpanded++;

				// Check if current node is goal
				if (domain.isGoal(node->getState()))
				{
					return;
				}

				topLevelActions[0].topLevelOpen.erase(remove(topLevelActions[0].topLevelOpen.begin(), topLevelActions[0].topLevelOpen.end(), node));

				vector<State> children = domain.successors(node->getState());
				res.nodesGenerated += children.size();

				Node* bestChild = NULL;

				for (State child : children)
				{
					Node* childNode = new Node(node->getGValue() + domain.getEdgeCost(child),
						domain.heuristic(child), domain.distance(child), eps, child, node);
					// Duplicate detection
					if (!duplicateDetection(childNode))
					{
						if (!bestChild)
						{
							bestChild = childNode;
						}
						else
						{
							if (childNode->getFHatValue() < bestChild->getFHatValue())
							{
								bestChild = childNode;
							}
						}

						open.push(childNode);
						openUclosed[child.hash()].push_back(childNode);
						topLevelActions[0].topLevelOpen.push_back(childNode);
					}
					else
						delete childNode;
				}

				// Backup the best child values to TLA via minimin
				// Use minimin to backup mean and var to TLA
				topLevelActions[0].mean = bestChild->getFHatValue();
				topLevelActions[0].var = pow((domain.distance(bestChild->getState()) / (domain.getBranchingFactor() + 1)), 2);
			}
			else
			{
				// Expand under TLA2
				Node* node = topLevelActions[1].topLevelOpen.back();

				// Also remove this node from open...
				priority_queue<Node*, vector<Node*>, CompareNodes> tmpOpen;
				while (!open.empty())
				{
					if (open.top()->getState() != node->getState())
					{
						tmpOpen.push(open.top());
					}
					open.pop();
				}
				open = tmpOpen;

				node->close();
				closed[node->getState().hash()].push_back(node);
				res.nodesExpanded++;

				// Check if current node is goal
				if (domain.isGoal(node->getState()))
				{
					return;
				}

				topLevelActions[1].topLevelOpen.erase(remove(topLevelActions[1].topLevelOpen.begin(), topLevelActions[1].topLevelOpen.end(), node));

				vector<State> children = domain.successors(node->getState());
				res.nodesGenerated += children.size();

				Node* bestChild = NULL;

				for (State child : children)
				{
					Node* childNode = new Node(node->getGValue() + domain.getEdgeCost(child),
						domain.heuristic(child), domain.distance(child), eps, child, node);
					// Duplicate detection
					if (!duplicateDetection(childNode))
					{
						if (!bestChild)
						{
							bestChild = childNode;
						}
						else
						{
							if (childNode->getFHatValue() < bestChild->getFHatValue())
							{
								bestChild = childNode;
							}
						}

						open.push(childNode);
						openUclosed[child.hash()].push_back(childNode);
						topLevelActions[1].topLevelOpen.push_back(childNode);
					}
					else
						delete childNode;
				}

				// Backup the best child values to TLA via minimin
				// Use minimin to backup mean and var to TLA
				topLevelActions[1].mean = bestChild->getFHatValue();
				topLevelActions[1].var = pow((domain.distance(bestChild->getState()) / (domain.getBranchingFactor() + 1)), 2);
			}

			expansions++;
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
		Node* start = new Node(0, 0, domain.distance(domain.getStartState()), eps, domain.getStartState(), NULL);
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
			generateTopLevelActions(start, res);

			// Expand some nodes until expnasion limit
			explore(lookahead, res);

			if (open.empty())
			{
				break;
			}

			// TODO: Learning?

			/*
			// Decisison strategy is to go to the node with the lowest expected value.
			// The expected value of a gaussian is the mean. Pick the TLA that minimizes the mean.
			sort(topLevelActions[0].topLevelOpen.begin(), topLevelActions[0].topLevelOpen.end(), CompareNodes());
			sort(topLevelActions[1].topLevelOpen.begin(), topLevelActions[1].topLevelOpen.end(), CompareNodes());

			Node* goalPrime = topLevelActions[0].topLevelOpen.back();

			if (topLevelActions[1].mean < topLevelActions[0].mean)
				goalPrime = topLevelActions[1].topLevelOpen.back();

			// Only move one step towards best on open
			while (goalPrime->getParent() != start)
				goalPrime = goalPrime->getParent();
			
			start = goalPrime;
			*/

			csernaDecision();
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

	void calculateCost(Node* solution, ResultContainer& res)
	{
		res.solutionFound = true;
		res.solutionCost = solution->getFValue();
	}
};