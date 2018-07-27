#g++ -g -std=c++11 -static ../cpp/main.cpp -o ../run.sh
g++ -g -std=c++11 -static ../cpp/generateTrees.cpp -o ../generateTrees.sh
g++ -g -std=c++11 -static ../cpp/lastIncrementalTests.cpp -o ../lastIncremental.sh
g++ -g -std=c++11 -static ../cpp/backupTestsDFS.cpp -o ../backupTestsDFS.sh
g++ -g -std=c++11 -static ../cpp/backupTestsAS.cpp -o ../backupTestsAS.sh
g++ -g -std=c++11 -static ../cpp/expansionTests.cpp -o ../expansionTests.sh
#chmod a+x ../run.sh
chmod a+x ../generateTrees.sh
chmod a+x ../lastIncremental.sh
chmod a+x ../backupTestsDFS.sh
chmod a+x ../backupTestsAS.sh
chmod a+x ../expansionTests.sh