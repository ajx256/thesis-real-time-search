#include <iostream>
#include <fstream>
#include <string>

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

	for (int i = 1; i <= n; i++)
	{
		string fileName = "b" + to_string(b) + "d" + to_string(depth) + "-" + to_string(i) + ".tw";
		ofstream out(dir + "/" + fileName);
		out << b << endl;
		out << depth << endl;
		out << rand() << endl;
		out.close();
	}
}