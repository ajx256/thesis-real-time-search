#pragma once
#include <vector>

using namespace std;

template <typename T>
class SlidingWindow {
public:
	SlidingWindow(unsigned int capacity = 100)
		: capacity(capacity) {}

	void push(T item) {
		if (window.size() == capacity)
		{
			// Erase the oldest item in the window (so index 0)
			window.erase(window.begin());
		}

		// Add the new item to the window
		window.push_back(item);
	}

	void clear()
	{
		while (!window.empty())
			window.pop_back();
	}

	int size()
	{
		return window.size();
	}

	typename vector<T>::iterator begin()
	{
		return window.begin();
	}

	typename vector<T>::iterator end()
	{
		return window.end();
	}

private:
	const unsigned int capacity;
	vector<T> window;
};