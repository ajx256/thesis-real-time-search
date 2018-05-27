#pragma once
#include <limits>
#include <cmath>

using namespace std;

class RandomGenerator
{
	long lo;
	long hi;
	unsigned long seed;
	long test;
	const long a = 48271.0;
	const long m = 2147483647.0;
	const long q = m / a;
	const long r = m % a;
public:
	RandomGenerator() : seed(0) {}
	RandomGenerator(unsigned long seed) : seed(seed) {};
	unsigned long getSeed() { return seed; }
	void setSeed(unsigned long s) { seed = s; }
	double random()
	{
		hi = seed / q;
		lo = seed % q;
		test = a * lo - r * hi;
		if (test > 0.0)
			seed = test;
		else
			seed = test + m;
		return ((double) seed / m);
	}
};