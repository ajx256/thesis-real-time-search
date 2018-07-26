#include "utility/SlidingWindow.h"
#include "utility/PriorityQueue.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

using namespace std;

struct Node
{
	int g;
	int h;
	bool open;

public:
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

int main()
{
	SlidingWindow<int> window(5);

	for (int i = 0; i < 10; i++)
		window.push(i);

	for (auto i : window)
		cout << i << " ";

	cout << endl;

	window.push(200);

	for (auto i : window)
		cout << i << " ";

	cout << endl;

	cout << "---------------------------------------------" << endl;

	PriorityQueue<Node*, CompareNodes> pq(5);

	srand(time(NULL));
	for (int i = 0; i < 5; i++)
	{
		int g = rand() % 100;
		int h = rand() % 100;

		Node* n = new Node(g, h);

		pq.push(n);
	}

	for (auto item : pq)
	{
		cout << item->getFValue() << " ";
	}

	cout << endl;

	for (int i = 0; i < 5; i++)
	{
		int g = rand() % 100;
		int h = rand() % 100;

		Node* n = new Node(g, h);

		pq.push(n);
	}

	for (auto item : pq)
	{
		cout << item->getFValue() << " ";
	}

	cout << endl;

	cout << pq.top()->getFValue() << endl;

	PriorityQueue<Node*, CompareNodes> pq1;

	for (int i = 0; i < 901; i++)
	{
		int g = rand() % 100;
		int h = rand() % 100;

		Node* n = new Node(g, h);

		pq1.push(n);
	}

	for (auto item : pq1)
	{
		cout << item->getFValue() << " ";
	}
}