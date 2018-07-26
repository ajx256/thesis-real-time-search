#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "../utility/Random.h"

class TreeWorld {
public:
	typedef double Cost;
	static constexpr Cost COST_MAX = std::numeric_limits<Cost>::max();

	class State {
	public:
		State() : depth(0), seedOffset(0) {}

		State(int depth, uint64_t seedOffset)
			: depth(depth), seedOffset(seedOffset) {}

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

		unsigned long long hash() const {
			return seedOffset;
		}

		std::string toString() const {
		std:string s = "";
			s += seedOffset + " " + depth;
			return s;
		}

		int getDepth() const {
			return depth;
		}

		uint64_t getSeedOffset() const {
			return seedOffset;
		}

	private:
		int depth;
		uint64_t seedOffset;
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

		for (int i = 0; i < maxEdgeCosts; i++)
		{
			costMap[i] = generator.random();
		}

		// Make the initial state
		startState = State(0, 0);
	}

	bool isGoal(const State& location) const {
		return location.getDepth() == maxDepth;
	}

	Cost distance(const State& state) const {
		return maxDepth - state.getDepth();
	}

	Cost heuristic(const State& state) const {
		return 0;
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
			State succ(state.getDepth() + 1, (state.getSeedOffset() * TreeWorld::branchingFactor) + i);
			successors.push_back(succ);
		}

		return successors;
	}

	bool safetyPredicate(const State& state) const { return true; }

	const State getStartState() const {
		return startState;
	}

	Cost getEdgeCost(State& state) const {
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

	State startState;
	int maxDepth;
	static int branchingFactor;
	unsigned long long seed;
	RandomGenerator generator;
	unordered_map<int, Cost> costMap;
	int maxEdgeCosts;
};

int TreeWorld::branchingFactor = 2;
