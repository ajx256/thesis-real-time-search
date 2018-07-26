#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

template <class T, class Compare = less<T> >
	class PriorityQueue {
	protected:
		vector<T> c;
		Compare comp;
		double capacity;

	public:
		explicit PriorityQueue(double maxCapacity = numeric_limits<double>::infinity(), const Compare& comp_ = Compare())
			: capacity(maxCapacity), comp(comp_)
		{
			make_heap(c.begin(), c.end(), comp);
		}

		void swapComparator(const Compare& comp_)
		{
			comp = comp_;
			make_heap(c.begin(), c.end(), comp);
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
					if (comp(c[i], c[worstIndex]))
					{
						worstIndex = i;
					}
				}

				// Check if the new item is better than the worst...
				if (comp(c[worstIndex], item))
				{
					// Erase the worst
					c.erase(c.begin() + worstIndex);
				}
				else
				{
					// If it isn't, do not add this item
					return;
				}

				// Remake the heap...
				make_heap(c.begin(), c.end(), comp);
			}
			
			c.push_back(item);
			push_heap(c.begin(), c.end(), comp);
		}

		void pop()
		{
			pop_heap(c.begin(), c.end(), comp);
			c.pop_back();
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
};