#pragma once
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "../utility/Random.h"
#include "../utility/SlidingWindow.h"

using namespace std;

class TreeWorld {
public:
	typedef double Cost;
	static constexpr Cost COST_MAX = std::numeric_limits<Cost>::max();
	
	class State {
	public:
		State() : depth(0), seedOffset(0) {}

		State(int depth, uint64_t seedOffset, int l)
			: depth(depth), seedOffset(seedOffset), label(l) {}

		friend std::ostream& operator<<(std::ostream& stream, const TreeWorld::State& state) {
			stream << state.toString();
			return stream;
		}

		bool operator==(const State& state) const {
			return ((seedOffset == state.seedOffset) && (depth == state.depth));
		}


		bool operator!=(const State& state) const {
			return ((seedOffset != state.seedOffset) || (depth != state.depth));
		}

		unsigned long long key() const {
			return seedOffset;
		}

		std::string toString() const {
		std:string s = "";
			s += std::to_string(seedOffset) + " " + std::to_string(depth);
			return s;
		}

		int getDepth() const {
			return depth;
		}

		uint64_t getSeedOffset() const {
			return seedOffset;
		}

		int getLabel() const
		{
			return label;
		}

		void markStart()
		{
			label = -1;
		}

	private:
		int depth;
		uint64_t seedOffset;
		int label;
	};
	
	struct HashState
	{
		std::size_t operator()(const State &s) const
		{
			return s.getSeedOffset();
		}
	};

	TreeWorld(std::istream& input) {
		// Read in the description of the tree
		string line;
		getline(input, line);
		TreeWorld::branchingFactor = stoi(line);
		getline(input, line);
		maxDepth = stoi(line);
		getline(input, line);
		seed = stoul(line);

		// Fill hash table of potential edge values in the tree
		maxEdgeCosts = 100000;
		generator.setSeed(seed);

		maxEdgeCost = -1;

		for (int i = 0; i < maxEdgeCosts; i++)
		{
			costMap[i] = generator.random();

			if (costMap[i] > maxEdgeCost)
				maxEdgeCost = costMap[i];
		}

		// Make the initial state
		startState = State(0, 0, -1);
	}

	bool isGoal(const State& location) const {
		return location.getDepth() == maxDepth;
	}

	Cost distance(const State& state) {
		// Check if the distance of this state has been updated
		if (correctedD.find(state) != correctedD.end())
		{
			return correctedD[state];
		}

		return maxDepth - state.getDepth();
	}

	Cost distanceErr(const State& state) {
		// Check if the distance error of this state has been updated
		if (correctedDerr.find(state) != correctedDerr.end())
		{
			return correctedDerr[state];
		}

		return maxDepth - state.getDepth();
	}

	Cost heuristic(const State& state) {
		// Check if the heuristic of this state has been updated
		if (correctedH.find(state) != correctedH.end())
		{
			return correctedH[state];
		}

		return 0;
	}

	Cost epsilonHGlobal()
	{
		return curEpsilonH;
	}

	Cost epsilonDGlobal()
	{
		return curEpsilonD;
	}

	void updateEpsilons()
	{
		// Listen, I don't think these were learned properly on this domain. My results were kind of funky, 
		// the heuristic is shit anyway. We're just gonna hard code this based on empirical evidence. 
		// Shut the fuck up about it.
		curEpsilonD = 0.0;

		curEpsilonH = 0.241;
	}

	void pushEpsilonHGlobal(double eps)
	{
		if (eps < 0)
			eps = 0;
		else if (eps > 1)
			eps = maxEdgeCost;

		epsilonHSum += eps;
		expansionCounter++;
	}

	void pushEpsilonDGlobal(double eps)
	{
		if (eps < 0)
			eps = 0;
		else if (eps > 1)
			eps = 1;

		epsilonDSum += eps;
		expansionCounter++;
	}

	void updateDistance(const State& state, Cost value) {
		correctedD[state] = value;
	}

	void updateDistanceErr(const State& state, Cost value) {
		correctedDerr[state] = value;
	}

	void updateHeuristic(const State& state, Cost value) {
		correctedH[state] = value;
	}

	int getBranchingFactor() const
	{
		return TreeWorld::branchingFactor;
	}

	std::vector<State> successors(const State& state) const {
		std::vector<State> successors;

		if (state.getDepth() == maxDepth)
			return successors;

		for (int i = 1; i <= branchingFactor; i++)
		{
			State succ(state.getDepth() + 1, (state.getSeedOffset() * TreeWorld::branchingFactor) + i, i);
			successors.push_back(succ);
		}

		return successors;
	}

	std::vector<State> predecessors(const State& state) const {
		std::vector<State> predecessors;

		if (state.getDepth() == 0)
			return predecessors;

		double dblOffset = state.getSeedOffset() / (double)branchingFactor;
		int offset = dblOffset;

		if (offset == dblOffset)
			offset = offset - 1;

		State pred(state.getDepth() - 1, offset, 0);
		predecessors.push_back(pred);

		return predecessors;
	}

	bool safetyPredicate(const State& state) const { return true; }

	const State getStartState() const {
		return startState;
	}

	Cost getEdgeCost(State state) {
		uint64_t i = state.getSeedOffset();
		i += seed;
		i %= maxEdgeCosts;
		return costMap.find(i)->second;
	}

	string getDomainInformation()
	{
		string info = "{ \"TreeSeed\": " + to_string(seed) + ", \"Depth\": " + 
			to_string(maxDepth) + ", \"BranchingFactor\": " + to_string(TreeWorld::branchingFactor) + " }";
		return info;
	}

	string getDomainName()
	{
		return "TreeWorld";
	}

	void initialize(string policy, int la)
	{
		epsilonDSum = 0;
		epsilonHSum = 0;
		expansionCounter = 0;

		expansionPolicy = policy;
		lookahead = la;
		correctedD.clear();
		correctedDerr.clear();
		correctedH.clear();
		expansionDelayWindow.clear();
	}

	void pushDelayWindow(int val)
	{
		expansionDelayWindow.push(val);
	}

	double averageDelayWindow()
	{
		if (expansionDelayWindow.size() == 0)
			return 1;

		double avg = 0;

		for (auto i : expansionDelayWindow)
		{
			avg += i;
		}

		avg /= expansionDelayWindow.size();

		return avg;
	}

	State startState;
	int maxDepth;
	static int branchingFactor;
	unsigned long long seed;
	RandomGenerator generator;
	unordered_map<int, Cost> costMap;
	unordered_map<State, Cost, HashState> correctedH;
	unordered_map<State, Cost, HashState> correctedD;
	unordered_map<State, Cost, HashState> correctedDerr;
	int maxEdgeCosts;
	SlidingWindow<int> expansionDelayWindow;

	double epsilonHSum;
	double epsilonDSum;
	double curEpsilonH;
	double curEpsilonD;
	double expansionCounter;
	Cost maxEdgeCost;

	string expansionPolicy;
	int lookahead;
};

int TreeWorld::branchingFactor = 2;
