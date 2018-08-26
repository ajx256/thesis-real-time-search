#pragma once
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

template <class Key, class Value>
class HashTable
{
	protected:
		struct Entry
		{
			Key key;
			Value value;
			Entry* next;
		};

		vector<int> randomTable;
		double numBuckets;
		double size;
		const double loadFactor = 0.75;
		Entry** buckets;

	public:
		HashTable()
		{
			numBuckets = 128;
			buckets = new Entry[numBuckets];
			for (int i = 0; i < numBuckets; i++)
			{
				buckets[i] = NULL;
			}

			srand(time(NULL));

			for (int i = 0; i < 256; i++)
			{
				randomTable.push_back(rand());
			}
		}

		HashTable(const HashTable& rhs)
		{
			numBuckets = rhs.numBuckets;
			randomTable = rhs.randomTable;
			size = rhs.size;

		}

		HashTable& operator=(const HashTable& rhs)
		{

		}

		~HashTable()
		{

		}

		void clear()
		{

		}

		Value& operator[](Key& k)
		{

		}


	private:
		void rehash()
		{

		}
};