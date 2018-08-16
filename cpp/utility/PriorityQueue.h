#pragma once
#include <vector>
#include <limits>
#include <algorithm>
#include <functional>

using namespace std;

template <class T>
	class PriorityQueue {
	protected:
		vector<T> c;
		std::function<bool(T,T)> comp;
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
			make_heap(c.begin(), c.end(), comp);
		}

		PriorityQueue(double maxCapacity, const std::function<bool(const T, const T)>& comp_)
			: capacity(maxCapacity), comp(comp_)
		{
			make_heap(c.begin(), c.end(), comp);
		}

		PriorityQueue(const std::function<bool(const T, const T)>& comp_)
			: comp(comp_)
		{
			capacity = numeric_limits<double>::infinity();
			make_heap(c.begin(), c.end(), comp);
		}

		PriorityQueue(double maxCapacity)
			: capacity(maxCapacity)
		{
			comp = lessThan;
			make_heap(c.begin(), c.end(), comp);
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
			make_heap(c.begin(), c.end(), comp);
		}

		void update()
		{
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
			if (c.empty())
			{
				return;
			}
			pop_heap(c.begin(), c.end(), comp);
			c.pop_back();
		}

		void remove(T item)
		{
			for (int i = 0; i < c.size(); i++)
			{
				if (c[i] == item)
				{
					c.erase(c.begin() + i);
					make_heap(c.begin(), c.end(), comp);
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
};