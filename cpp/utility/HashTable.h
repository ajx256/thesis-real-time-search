#pragma once
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;
// TODO: This hashtable doesn't really work. Tabulation hashing results in non-deterministic search behavior. Very odd.
// Perhaps due to duplicates not being detected (collisions). Compare number of collisions to another, more basic hash function.
template <class Key, class Value>
class HashTable
{
	protected:
		struct Entry
		{
			public:
				Key key;
				Value value;
				Entry* next;

				Entry()
				{ 
					next = NULL;
				}

				Entry(Key k, Value v) : key(k), value(v) { next = NULL; }
		};

		vector<int> randomTable;
		int numBuckets;
		int size;
		const double loadFactor = 0.75;
		Entry** buckets;

		unsigned int hash(Key k)
		{
			unsigned int h = 0;

			unsigned long long key = k.key();

			// For each byte in the key...
			for (int i = 7; i >= 0; i--)
			{
				unsigned int byte = (key >> (i * 8)) & 0x000000FF;
				h = leftRotate(h, 1);
				h = h ^ randomTable[byte];
			}

			return h % numBuckets;
		}

	public:
		struct iterator
		{
			private:
				Entry* e;
				HashTable<Key, Value>* owningTable;

			public:
				iterator(HashTable<Key, Value>* table) : owningTable(table), e(NULL) {}

				iterator(HashTable<Key, Value>* table, Entry* entry) : owningTable(table), e(entry) {}

				Entry& operator*()
				{
					return *e;
				}

				Entry* operator->()
				{
					return e;
				}

				bool operator==(const iterator& it)
				{
					return e == it.e && owningTable == it.owningTable;
				}

				bool operator!=(const iterator& it)
				{
					return e != it.e || owningTable != it.owningTable;
				}

				iterator& operator++(int)
				{
					// Try to advance to the next entry in this bucket...
					if (e->next != NULL)
					{
						e = e->next;
					}
					else
					{
						// Otherwise we have to go to the next bucket...
						unsigned int curBucket = owningTable->hash(e->key) + 1;

						// Find the first non-empty bucket...
						while (curBucket < owningTable->bucketCount())
						{
							if (owningTable->buckets[curBucket] != NULL)
							{
								e = owningTable->buckets[curBucket];
								return *this;
							}

							curBucket += 1;
						}
						
						// Reached the end of the hash table, return end()
						e = NULL;
					}

					return *this;
				}
		};

		HashTable()
		{
			numBuckets = 128;
			buckets = new Entry*[numBuckets];
			size = 0;

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
			copyCode(rhs);
		}

		HashTable& operator=(const HashTable& rhs)
		{
			if (this == &rhs)
			{
				return *this;
			}

			destructCode();
			copyCode(rhs);

			return *this;
		}

		~HashTable()
		{
			destructCode();
			size = 0;
			randomTable.clear();
		}

		void clear()
		{
			destructCode();
			buckets = new Entry*[numBuckets];
			size = 0;

			for (int i = 0; i < numBuckets; i++)
			{
				buckets[i] = NULL;
			}
		}

		bool empty()
		{
			return size == 0;
		}

		void insert(Key k, Value v)
		{
			unsigned int h = hash(k);
			
			// Make sure the item doesn't already exist...
			Entry* cur = buckets[h];

			while (cur != NULL)
			{
				if (cur->key == k)
					return;
				cur = cur->next;
			}

			// If you're here, this key ought to be inserted...
			Entry* entry = new Entry(k, v);
			entry->next = buckets[h];
			buckets[h] = entry;
			size++;

			if (size / numBuckets >= loadFactor)
				rehash();
		}

		Value& operator[](const Key& k)
		{
			// First try to find it...
			unsigned int h = hash(k);

			Entry* cur = buckets[h];

			while (cur != NULL)
			{
				if (cur->key == k)
				{
					return cur->value;
				}
				cur = cur->next;
			}

			// Not in the table, add it...
			Value v;
			Entry* entry = new Entry(k, v);
			entry->next = buckets[h];
			buckets[h] = entry;
			size++;

			if (size / numBuckets >= loadFactor)
				rehash();

			return entry->value;
		}

		void erase(Key k)
		{
			unsigned int h = hash(k);

			Entry* cur = buckets[h];
			
			if (cur->key == k)
			{
				buckets[h] = cur->next;
				delete cur;
				size--;
				return;
			}

			Entry* prev = cur;
			cur = cur->next;

			while (cur != NULL)
			{
				if (cur->key == k)
				{
					prev->next = cur->next;
					delete cur;
					size--;
					return;
				}
				prev = cur;
				cur = cur->next;
			}
		}

		int getSize()
		{
			return size;
		}

		int bucketCount()
		{
			return numBuckets;
		}

		iterator begin()
		{
			// Find the first non-empty bucket...
			unsigned int curBucket = 0;

			while (curBucket < numBuckets)
			{
				if (buckets[curBucket] != NULL)
				{
					break;
				}

				curBucket += 1;
			}

			if (curBucket < numBuckets)
			{
				return iterator(this, buckets[curBucket]);
			}
			else
			{
				return iterator(this, NULL);
			}
		}

		iterator end()
		{
			return iterator(this, NULL);
		}

		iterator find(const Key& k)
		{
			unsigned int h = hash(k);

			Entry* cur = buckets[h];

			while (cur != NULL)
			{
				if (cur->key == k)
					return iterator(this, cur);
				
				cur = cur->next;
			}

			return iterator(this, NULL);
		}

	private:
		void rehash()
		{
			// Make the new table...
			numBuckets *= 2;
			Entry** newTable = new Entry*[numBuckets];

			for (int i = 0; i < numBuckets; i++)
			{
				newTable[i] = NULL;
			}

			// Insert everything into the new table...
			for (int i = 0; i < numBuckets / 2; i++)
			{
				Entry* cur = buckets[i];
				while (cur != NULL)
				{
					Entry* entry = new Entry(cur->key, cur->value);
					unsigned int h = hash(entry->key);
					entry->next = newTable[h];
					newTable[h] = entry;

					cur = cur->next;
				}
			}

			// Delete the old table
			numBuckets = numBuckets / 2;
			destructCode();
			numBuckets = numBuckets * 2;

			buckets = newTable;
		}

		unsigned int leftRotate(std::size_t n, unsigned int d) const
		{
			return (n << d) | (n >> (32 - d));
		}

		void destructCode()
		{
			for (int i = 0; i < numBuckets; i++)
			{
				Entry* cur = buckets[i];
				while (cur != NULL)
				{
					Entry* next = cur->next;
					delete cur;
					cur = next;
				}
			}
			delete[] buckets;
		}

		void copyCode(const HashTable& rhs)
		{
			numBuckets = rhs.numBuckets;
			randomTable = rhs.randomTable;
			size = rhs.size;

			buckets = new Entry*[numBuckets];

			for (int i = 0; i < numBuckets; i++)
			{
				buckets[i] = NULL;
			}

			for (int i = 0; i < numBuckets; i++)
			{
				Entry* cur = rhs.buckets[i];
				while (cur != NULL)
				{
					Entry* entry = new Entry(cur->key, cur->value);
					entry->next = buckets[i];
					buckets[i] = entry;

					cur = cur->next;
				}
			}
		}
};