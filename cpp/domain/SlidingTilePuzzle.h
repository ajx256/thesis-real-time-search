#pragma once
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include "../utility/SlidingWindow.h"

using namespace std;

class SlidingTilePuzzle {
public:
	typedef double Cost;
	static constexpr Cost COST_MAX = std::numeric_limits<Cost>::max();

	class State {
	public:
		State() {}

		State(std::vector<std::vector<int> > b, char l) : board(b), label(l) {}
		
		friend std::ostream& operator<<(std::ostream& stream, const SlidingTilePuzzle::State& state) {
			for (int r = 0; r < state.getBoard().size(); r++)
			{
				for (int c = 0; c < state.getBoard()[r].size(); c++)
				{
					stream << std::setw(3) << state.getBoard()[r][c] << " ";
				}
				stream << endl;
			}
			return stream;
		}

		bool operator==(const State& state) const {
			return board == state.getBoard();
		}


		bool operator!=(const State& state) const {
			return board != state.getBoard();
		}

		unsigned long long hash() const {
			// This will provide a unique hash for every state in the 15 puzzle,
			// Other puzzle variants may/will see collisions...
			unsigned long long val = 0;
			for (int r = 0; r < board.size(); r++)
			{
				for (int c = 0; c < board[r].size(); c++)
				{
					unsigned long long index = (board.size() * r) + c;
					val |= index << (board[r][c]);
				}
			}
			return val;
		}

		std::string toString() const {
			std::string s = "";
			for (int r = 0; r < board.size(); r++)
			{
				for (int c = 0; c < board[r].size(); c++)
				{
					s += std::to_string(board[r][c]) + " ";
				}
				s += "\n";
			}
			return s;
		}

		std::vector<std::vector<int> > getBoard() const {
			return board;
		}

		char getLabel() const
		{
			return label;
		}

	private:
		std::vector<std::vector<int> > board;
		char label;
	};

	struct HashState
	{
		std::size_t operator()(const State &s) const
		{
			unsigned long long val = 0;
			for (int r = 0; r < s.getBoard().size(); r++)
			{
				for (int c = 0; c < s.getBoard()[r].size(); c++)
				{
					unsigned long long index = (s.getBoard().size() * r) + c;
					val |= index << (s.getBoard()[r][c]);
				}
			}
			return val;
		}
	};

	SlidingTilePuzzle(std::istream& input) {
		// Get the dimensions of the puzzle
		string line;
		getline(input, line);
		stringstream ss(line);
		// Get the first dimension...
		ss >> size;
		// We don't give a shit about the second dimension, 
		// because every puzzle should be square.

		// Skip the next line
		getline(input, line);

		// Initialize the nxn puzzle board
		std::vector<int> rows(size, 0);
		std::vector<std::vector<int> > board(size, rows);
		startBoard = board;
		endBoard = board;

		// Following lines are the input puzzle...
		int r = 0;
		int c = 0;

		for (int i = 0; i < size * size; i++)
		{
			c = i % size;

			getline(input, line);
			int tile;
			stringstream ss2(line);
			ss2 >> tile;

			startBoard[r][c] = tile;

			if (c >= size - 1)
			{
				r++;
			}
		}

		// Skip the next line
		getline(input, line);

		// Following lines are the goal puzzle...
		r = 0;
		c = 0;

		for (int i = 0; i < size * size; i++)
		{
			c = i % size;

			getline(input, line);
			int tile;
			stringstream ss2(line);
			ss2 >> tile;

			endBoard[r][c] = tile;

			if (c >= size - 1)
			{
				r++;
			}
		}

		startState = State(startBoard, 's');
	}

	bool isGoal(const State& s) const {
		if (s.getBoard() == endBoard)
		{
			return true;
		}

		return false;
	}

	Cost distance(const State& state) {
		// Check if the distance of this state has been updated
		if (correctedD.find(state) != correctedD.end())
		{
			return correctedD[state];
		}

		Cost d = manhattanDistance(state);

		correctedD[state] = d;

		return correctedD[state];
	}

	Cost distanceErr(const State& state) {
		// Check if the distance error of this state has been updated
		if (correctedDerr.find(state) != correctedDerr.end())
		{
			return correctedDerr[state];
		}

		Cost derr = manhattanDistance(state);

		correctedDerr[state] = derr;

		return correctedDerr[state];
	}

	Cost heuristic(const State& state) {
		// Check if the heuristic of this state has been updated
		if (correctedH.find(state) != correctedH.end())
		{
			return correctedH[state];
		}

		Cost h = manhattanDistance(state);

		correctedH[state] = h;

		return correctedH[state];
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
		if (expansionCounter == 0)
		{
			curEpsilonD = 0;
			curEpsilonH = 0;

			return;
		}

		curEpsilonD = epsilonDSum / expansionCounter;

		curEpsilonH = epsilonHSum / expansionCounter;
	}

	void pushEpsilonHGlobal(double eps)
	{
		if (eps < 0)
			eps = 0;
		else if (eps > 1)
			eps = 1;

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

	Cost manhattanDistance(const State& state) const {
		Cost manhattanSum = 0;

		for (int r = 0; r < size; r++) {
			for (int c = 0; c < size; c++) {
				auto value = state.getBoard()[r][c];
				if (value == 0) {
					continue;
				}

				manhattanSum += abs(value / size - r) + abs(value % size - c);
			}
		}

		return manhattanSum;
	}

	double getBranchingFactor() const
	{
		return 2.13;
	}

	void moveUp(std::vector<State>& succs, std::vector<std::vector<int> > board) const
	{
		int r = 0;
		int c = 0;
		// Find the location of the blank space
		bool found = false;
		for (r = 0; r < size; r++)
		{
			for (c = 0; c < size; c++)
			{
				if (board[r][c] == 0)
				{
					found = true;
					break;
				}
			}

			if (found)
				break;
		}

		// Now try to move the blank tile up one row...
		if (r - 1 > -1)
		{
			std::swap(board[r][c], board[r - 1][c]);
			succs.push_back(State(board, 'U'));
		}
	}

	void moveDown(std::vector<State>& succs, std::vector<std::vector<int> > board) const
	{
		int r = 0;
		int c = 0;
		// Find the location of the blank space
		bool found = false;
		for (r = 0; r < size; r++)
		{
			for (c = 0; c < size; c++)
			{
				if (board[r][c] == 0)
				{
					found = true;
					break;
				}
			}

			if (found)
				break;
		}

		// Now try to move the blank tile down one row...
		if (r + 1 < size)
		{
			std::swap(board[r][c], board[r + 1][c]);
			succs.push_back(State(board, 'D'));
		}
	}

	void moveLeft(std::vector<State>& succs, std::vector<std::vector<int> > board) const
	{
		int r = 0;
		int c = 0;
		// Find the location of the blank space
		bool found = false;
		for (r = 0; r < size; r++)
		{
			for (c = 0; c < size; c++)
			{
				if (board[r][c] == 0)
				{
					found = true;
					break;
				}
			}

			if (found)
				break;
		}

		// Now try to move the blank tile left one column...
		if (c - 1 > -1)
		{
			std::swap(board[r][c], board[r][c - 1]);
			succs.push_back(State(board, 'L'));
		}
	}

	void moveRight(std::vector<State>& succs, std::vector<std::vector<int> > board) const
	{
		int r = 0;
		int c = 0;
		// Find the location of the blank space
		bool found = false;
		for (r = 0; r < size; r++)
		{
			for (c = 0; c < size; c++)
			{
				if (board[r][c] == 0)
				{
					found = true;
					break;
				}
			}

			if (found)
				break;
		}

		// Now try to move the blank tile left one column...
		if (c + 1 < size)
		{
			std::swap(board[r][c], board[r][c + 1]);
			succs.push_back(State(board, 'R'));
		}
	}

	std::vector<State> successors(const State& state) const {
		std::vector<State> successors;

		// Don't allow inverse actions, to cut down on branching factor

		if (state.getLabel() != 'D')
			moveUp(successors, state.getBoard());
		if (state.getLabel() != 'U')
			moveDown(successors, state.getBoard());
		if (state.getLabel() != 'R')
			moveLeft(successors, state.getBoard());
		if (state.getLabel() != 'L')
			moveRight(successors, state.getBoard());

		return successors;
	}

	std::vector<State> predecessors(const State& state) const {
		std::vector<State> predecessors;

		moveUp(predecessors, state.getBoard());
		moveDown(predecessors, state.getBoard());
		moveLeft(predecessors, state.getBoard());
		moveRight(predecessors, state.getBoard());

		return predecessors;
	}

	bool safetyPredicate(const State& state) const { return true; }

	const State getStartState() const {
		return startState;
	}

	Cost getEdgeCost(State state) {
		return 1;
	}

	string getDomainInformation()
	{
		string info = "{ \"Domain\": \"Sliding Tile Puzzle\", \"Dimensions\": " + 
			std::to_string(size) + "x" + std::to_string(size) + " }";
		return info;
	}

	string getDomainName()
	{
		return "SlidingTilePuzzle";
	}

	void initialize(string policy, int la)
	{
		epsilonDSum = 0;
		epsilonHSum = 0;
		expansionCounter = 0;

		expansionPolicy = policy;
		lookahead = la;
		correctedD.clear();
		correctedH.clear();
		correctedDerr.clear();
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

	std::vector<std::vector<int> > startBoard;
	std::vector<std::vector<int> > endBoard;
	int size;
	State startState;
	SlidingWindow<int> expansionDelayWindow;
	unordered_map<State, Cost, HashState> correctedH;
	unordered_map<State, Cost, HashState> correctedD;
	unordered_map<State, Cost, HashState> correctedDerr;

	double epsilonHSum;
	double epsilonDSum;
	double curEpsilonH;
	double curEpsilonD;
	double expansionCounter;

	string expansionPolicy;
	int lookahead;
};