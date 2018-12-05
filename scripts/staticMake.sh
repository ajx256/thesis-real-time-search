rm ../generateTrees
rm ../lastIncremental
rm ../backupTestsDFS
rm ../backupTestsAS
rm ../expansionTests

g++ -std=c++11 -static ../cpp/generateTrees.cpp -o ../generateTrees
g++ -std=c++11 -static ../cpp/lastIncrementalTests.cpp -o ../lastIncremental
g++ -std=c++11 -static ../cpp/backupTestsDFS.cpp -o ../backupTestsDFS
g++ -std=c++11 -static ../cpp/backupTestsAS.cpp -o ../backupTestsAS
g++ -std=c++11 -static ../cpp/expansionTests.cpp -o ../expansionTests

chmod a+x ../generateTrees
chmod a+x ../lastIncremental
chmod a+x ../backupTestsDFS
chmod a+x ../backupTestsAS
chmod a+x ../expansionTests
