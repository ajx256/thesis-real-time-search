#include <iostream>
#include <fstream>
#include <string>
#include <climits>
using namespace std;

int main(int argc, char** argv)
{
	if (argc != 5)
	{
		cout << "./generateTrees <maxDepth> <maxBranchingFactor> <number of instances> <world instance dir>" << endl;
		exit(1);
	}

	int depth = stoi(argv[1]);
	int b = stoi(argv[2]);
	int n = stoi(argv[3]);
	string dir = argv[4];
	srand(time(NULL));

	for (int i = 1; i <= n; i++)
	{
		string fileName = "b" + to_string(b) + "d" + to_string(depth) + "-" + to_string(i) + ".tw";

		// If this instance was already created, skip it
		ifstream in(dir + "/" + fileName);
		if (in.good())
		{
			in.close();
			continue;
		}
		in.close();

		ofstream out(dir + "/" + fileName);
		out << b << endl;
		out << depth << endl;
		out << rand() % ULONG_MAX << endl;
		out.close();
	}
}
