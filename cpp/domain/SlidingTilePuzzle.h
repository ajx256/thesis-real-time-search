#pragma once
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
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
			stream << state.toString();
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
			return true;
		return false;
	}

	Cost distance(const State& state) {
		// Check if the distance of this state has been updated
		if (correctedD.find(state) != correctedD.end())
		{
			return correctedD[state];
		}

		return manhattanDistance(state);
	}

	Cost heuristic(const State& state) {
		// Check if the heuristic of this state has been updated
		if (correctedH.find(state) != correctedH.end())
		{
			return correctedH[state];
		}

		return manhattanDistance(state);
	}

	void updateDistance(const State& state, Cost value) {
		correctedD[state] = value;
	}

	void updateHeuristic(const State& state, Cost value) {
		correctedH[state] = value;
	}

	Cost manhattanDistance(const State& state) const {
		int manhattanSum = 0;

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

	int getBranchingFactor() const
	{
		return 4;
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
		moveUp(successors, state.getBoard());
		moveDown(successors, state.getBoard());
		moveLeft(successors, state.getBoard());
		moveRight(successors, state.getBoard());

		/*
		for (State s : successors)
		{
			cout << s.getLabel() << endl;
			cout << "FROM: " << endl;
			cout << state << endl;
			cout << endl;
			cout << "TO: " << endl;
			cout << s << endl;
			cout << "------------------------------------------" << endl;
		}
		*/

		return successors;
	}

	std::vector<State> predecessors(const State& state) const {
		return successors(state);
	}

	bool safetyPredicate(const State& state) const { return true; }

	const State getStartState() const {
		return startState;
	}

	Cost getEdgeCost(State& state) const {
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
};