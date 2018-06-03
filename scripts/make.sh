g++ -g -std=c++11 -static ../cpp/main.cpp -o ../run.sh
g++ -g -std=c++11 -static ../cpp/generateTrees.cpp -o ../generateTrees.sh
g++ -g -std=c++11 -static ../cpp/lastIncrementalTests.cpp -o ../lastIncremental.sh
g++ -g -std=c++11 -static ../cpp/backupTests.cpp -o ../backupTests.sh
chmod a+x ../run.sh
chmod a+x ../generateTrees.sh
chmod a+x ../lastIncremental.sh
chmod a+x ../backupTests.sh
