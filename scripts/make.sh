g++ -g -std=c++11 -static ../cpp/generateTrees.cpp -o ../generateTrees
g++ -g -std=c++11 -static ../cpp/lastIncrementalTests.cpp -o ../lastIncremental
g++ -g -std=c++11 -static ../cpp/backupTestsDFS.cpp -o ../backupTestsDFS
g++ -g -std=c++11 -static ../cpp/backupTestsAS.cpp -o ../backupTestsAS
g++ -g -std=c++11 -static ../cpp/expansionTests.cpp -o ../expansionTests

chmod a+x ../generateTrees
chmod a+x ../lastIncremental
chmod a+x ../backupTestsDFS
chmod a+x ../backupTestsAS
chmod a+x ../expansionTests