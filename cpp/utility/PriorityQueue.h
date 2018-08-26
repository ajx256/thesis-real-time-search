#pragma once
#include <vector>
#include <limits>
#include <functional>

using namespace std;

template <class T>
class PriorityQueue {
protected:
	vector<T> c;
	std::function<bool(T, T)> comp;
	double capacity;

	static bool lessThan(const T n1, const T n2)
	{
		return n1 < n2;
	}

public:
	PriorityQueue()
	{
		capacity = numeric_limits<double>::infinity();
		comp = lessThan;
		makeHeap();
	}

	PriorityQueue(double maxCapacity, const std::function<bool(const T, const T)>& comp_)
		: capacity(maxCapacity), comp(comp_)
	{
		makeHeap();
	}

	PriorityQueue(const std::function<bool(const T, const T)>& comp_)
		: comp(comp_)
	{
		capacity = numeric_limits<double>::infinity();
		makeHeap();
	}

	PriorityQueue(double maxCapacity)
		: capacity(maxCapacity)
	{
		comp = lessThan;
		makeHeap();
	}

	PriorityQueue(const PriorityQueue<T>& pq)
	{
		c = pq.c;
		comp = pq.comp;
		capacity = pq.capacity;
	}

	PriorityQueue<T>& operator=(const PriorityQueue<T>& rhs)
	{
		if (&rhs == this)
			return *this;
		c = rhs.c;
		comp = rhs.comp;
		capacity = rhs.capacity;
		return *this;
	}

	void swapComparator(const std::function<bool(const T, const T)>& comp_)
	{
		comp = comp_;
		makeHeap();
	}

	void update(T item)
	{
		for (int i = 0; i < c.size(); i++)
		{
			if (c[i] == item)
			{
				if (comp(c[i], c[parent(i)]))
					pullUp(i);
				else
					pushDown(i);
				break;
			}
		}
	}

	bool empty() const
	{
		return c.empty();
	}

	size_t size() const
	{
		return c.size();
	}

	const T top() const
	{
		return c.front();
	}

	void push(const T item)
	{
		// If the size is at maximum capacity, find the worst item in the queue
		if (c.size() == capacity)
		{
			int worstIndex = 0;
			for (int i = 1; i < c.size(); i++)
			{
				if (comp(c[worstIndex], c[i]))
				{
					worstIndex = i;
				}
			}

			// Check if the new item is better than the worst...
			if (comp(item, c[worstIndex]))
			{
				// Erase the worst
				swap(c[worstIndex], c[last()]);
				c.erase(c.begin() + last());
				if (comp(c[worstIndex], c[parent(worstIndex)]))
					pullUp(worstIndex);
				else
					pushDown(worstIndex);
			}
			else
			{
				// If it isn't, do not add this item
				return;
			}
		}

		c.push_back(item);
		pullUp(last());
	}

	void pop()
	{
		if (c.empty())
		{
			return;
		}
		swap(c[0], c[last()]);
		c.erase(c.begin() + last());
		pushDown(0);
	}

	void remove(T item)
	{
		for (int i = 0; i < c.size(); i++)
		{
			if (c[i] == item)
			{
				swap(c[i], c[last()]);
				c.erase(c.begin() + last());
				if (comp(c[i], c[parent(i)]))
					pullUp(i);
				else
					pushDown(i);
				break;
			}
		}
	}

	void clear()
	{
		while (!c.empty())
		{
			c.pop_back();
		}
	}

	typename vector<T>::iterator begin()
	{
		return c.begin();
	}

	typename vector<T>::iterator end()
	{
		return c.end();
	}

	typename vector<T>::iterator find(T item)
	{
		for (int i = 0; i < c.size(); i++)
		{
			if (c[i] == item)
				return c.begin() + i;
		}

		return c.end();
	}
private:
	int last()
	{
		return c.size() - 1;
	}

	int parent(int i)
	{
		return (i - 1) / 2;
	}

	int rightChild(int i)
	{
		return 2 * i + 2;
	}

	int leftChild(int i)
	{
		return 2 * i + 1;
	}

	void pullUp(int i)
	{
		if (comp(c[i], c[parent(i)]))
		{
			swap(c[i], c[parent(i)]);
			pullUp(parent(i));
		}
	}

	void pushDown(int i)
	{
		int smallesti = i;

		if (rightChild(i) < c.size() && comp(c[rightChild(i)], c[smallesti]))
		{
			smallesti = rightChild(i);
		}

		if (leftChild(i) < c.size() && comp(c[leftChild(i)], c[smallesti]))
		{
			smallesti = leftChild(i);
		}

		if (smallesti != i)
		{
			swap(c[i], c[smallesti]);
			pushDown(smallesti);
		}
	}

	void makeHeap()
	{
		for (int i = (c.size() / 2) - 1; i >= 0; i--)
		{
			pushDown(i);
		}
	}
};