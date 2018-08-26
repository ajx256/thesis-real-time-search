#include "../utility/SlidingWindow.h"
#include "../utility/PriorityQueue.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <functional>

using namespace std;

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

bool comparesNodesF(const Node* n1, const Node* n2)
{
	// Tie break on heuristic
	if (n1->getFValue() == n2->getFValue())
		return n1->getGValue() > n2->getGValue();
	return n1->getFValue() < n2->getFValue();
}

bool compareNodesH(const Node* n1, const Node* n2)
{
	return n1->getHValue() < n2->getHValue();
}

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

	PriorityQueue<Node*> pq(5, comparesNodesF);

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

	PriorityQueue<Node*> pq1(comparesNodesF);

	for (int i = 0; i < 901; i++)
	{
		int g = rand() % 100;
		int h = rand() % 100;

		Node* n = new Node(g, h);

		pq1.push(n);
	}

	pq1.swapComparator(compareNodesH);

	cout << endl;
	cout << endl;

	while (!pq1.empty())
	{
		cout << pq1.top()->getHValue() << " ";
		pq1.pop();
	}

	cout << endl;
	cout << "------- C++ Tie Breaking Test --------" << endl;
	PriorityQueue<Node*> pq2;
	pq2.swapComparator(compareNodesH);

	for (int i = 0; i < 25; i++)
	{
		int h = 1;
		int g = 5;
		if (i % 2 == 0)
		{
			g = 3;
			h = 2;
		}
		Node* n = new Node(g, h);
		n->label = i + 65;
		pq2.push(n);
		cout << pq2.top()->label << endl;
	}
	cout << "------------TEST SWAP COMP------------" << endl;
	for (int i = 0; i < 50; i++)
	{
		pq2.swapComparator(comparesNodesF);
		for (Node* n : pq2)
		{
			cout << n->label << " ";
		}
		cout << endl;
		pq2.swapComparator(compareNodesH);
		for (Node* n : pq2)
		{
			cout << n->label << " ";
		}
		cout << endl;
		cout << "---------------------------" << endl;
	}
}