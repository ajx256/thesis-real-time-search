#pragma once
#include <functional>
#include "DecisionAlgorithm.h"
#include "../utility/PriorityQueue.h"

using namespace std;

template <class Domain, class Node, class TopLevelAction>
class KBestBackup : public DecisionAlgorithm<Domain, Node, TopLevelAction>
{
	typedef typename Domain::State State;
	typedef typename Domain::Cost Cost;

public:
	KBestBackup(Domain& domain, double k, string beliefType, double lookahead)
		: domain(domain), k(k), beliefType(beliefType), lookahead(lookahead)
	{}

	Node* backup(PriorityQueue<Node*>& open, vector<TopLevelAction>& tlas, Node* start)
	{
		kBestDecision(tlas);

		// Take the TLA with the lowest expected minimum path cost
		TopLevelAction lowestExpectedPathTLA = tlas[0];
		for (TopLevelAction tla : tlas)
		{
			if (tla.expectedMinimumPathCost < lowestExpectedPathTLA.expectedMinimumPathCost)
				lowestExpectedPathTLA = tla;
		}

		Node* goalPrime = lowestExpectedPathTLA.topLevelNode;

		return goalPrime;
	}

private:

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

	void kBestDecision(vector<TopLevelAction>& tlas)
	{
		// The K-Best decision assumes that the only nodes within the subtrees of the TLAs are the k-best frontier nodes
		// on their opened lists. Find them.
		for (TopLevelAction& tla : tlas)
		{
			tla.kBestNodes.clear();

			if (!tla.open.empty())
			{
				// If this TLA has unique, probably optimal subtrees beneath it, it is valid

				int i = 0;
				// Add to the best k nodes while i < k and non-selected nodes exist on the frontier
				while (i < k && !tla.open.empty())
				{
					Node* best = tla.open.top();
					tla.open.pop();

					// Make this node's PDF a discrete distribution...
					if (beliefType == "normal")
					{
						best->distribution = DiscreteDistribution(100, best->getFValue(), best->getFHatValue(),
							best->getDValue(), best->getFHatValue() - best->getFValue());
					}
					else
					{
						best->distribution = DiscreteDistribution(100, best->getGValue(), best->getDValue(), domain.getBranchingFactor());
					}

					tla.kBestNodes.push_back(best);
					i++;
				}

				// Now that k-best are selected, perform Cserna backup
				csernaBackup(tla);
			}
			else
			{
				// This TLA has no unique subtrees beneath it, thus can be pruned...
				tla.expectedMinimumPathCost = numeric_limits<double>::infinity();
				tla.belief = DiscreteDistribution(100, numeric_limits<double>::infinity());
			}
		}
	}

protected:
	Domain& domain;
	double k;
	string beliefType;
	double lookahead;
};