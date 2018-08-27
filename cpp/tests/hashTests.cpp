#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../utility/HashTable.h"

struct Node
{
	int g;
	int h;
	bool open;

public:
	char label;
	int getGValue() const { return g; }
	int getHValue() const { return h; }
	int getFValue() const { return g + h; }
	void setHValue(int val) { h = val; }
	void setGValue(int val) { g = val; }
	bool onOpen() { return open; }
	void close() { open = false; }
	void reopen() { open = true; }

	Node(int g, int h)
		: g(g), h(h) {}
};

struct State
{
	int rep;

	State(int r) : rep(r) {}

	unsigned int key()
	{
		return rep;
	}

	bool operator==(const State& s)
	{
		return rep == s.rep;
	}
};

int main()
{
	cout << "--------------Test Hash Table--------------------" << endl;
	HashTable<State, Node*> ht;

	for (int i = 0; i < 4000; i++)
	{
		srand(time(NULL));
		int g = rand() % 100 + 1;
		int h = rand() % 50;
		Node* n = new Node(g, h);
		State s = State(i);
		ht.insert(s, n);
		if (i == 50)
			cout << g + h << endl;
	}
	Node* n = new Node(0, -1);
	ht.insert(State(50), n);

	cout << ht[State(50)]->getFValue() << endl;
	
	ht[State(7000)] = n;

	cout << ht[State(7000)]->getFValue() << endl;
	cout << ht.find(State(7000))->value->getFValue() << endl;

	for (HashTable<State, Node*>::iterator it = ht.begin(); it != ht.end(); it++)
	{
		delete it->value;
	}
}