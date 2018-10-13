TO BUILD:

cd scripts/

./make.sh

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TO RUN TESTS FOR MY THESIS AND AAAI SUBMISSION:

cd scripts/testHarnesses/

To run backup rule tests for last incremental decision:

./lastIncrementalDecisionHarness.sh 30000

Notes:

This script will perform searches using minimin, Bellman, Nancy, Cserna, and k-Best backups on 30000 instances of a random binary tree of depth 10.
The script will spawn 12 background process at a time to perform the searches.  The 30000 instances are chosen from the world directory, starting with instance 1.

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

To run backup rule tests with DFS expansion:

On 1000 random trees of branching factor 2 and depth 100:

./backupTestHarness.sh 1 1000 DFS <# of processes> TreeWorld 2 100 2 3 7 10

*Sliding Tile Puzzle can be run in a similar way, but DFS expansion strategy would be horrible and take forever*

Notes:

<# of processes> is the number of background processes the script will run at a time.
I recommend only using as many as there are cores on your machine.  

You can queue up as many expansion limits as you would like, or split them up among different machines, ie:

On 12-core machine run depth limits 2, 3:

./backupTestHarness.sh 1 1000 DFS 12 TreeWorld 2 100 2 3

On 12-core machine run depth limits 7, 10:

./backupTestHarness.sh 1 1000 DFS 12 TreeWorld 2 100 7 10

You can also split instances between machines using <starting instance #> and <# of instances to test>, ie:

On 12-core machine run random tree instances 1 through 500:

./backupTestHarness.sh 1 500 DFS 12 TreeWorld 2 100 2 3 7 10

On 12-core machine run random tree instances 501 through 1000:

./backupTestHarness.sh 501 500 DFS 12 TreeWorld 2 100 2 3 7 10

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

To run backup rule tests with A* expansion:

On 1000 random trees of branching factor 2 and depth 100:

./backupTestHarness.sh 1 1000 AS <# of processes> TreeWorld 2 100 10 30 100 300 1000

On Korf's 100 puzzle instances:

./backupTestHarness.sh 1 100 AS <# of processes> SlidingPuzzle 4 10 30 100 300 1000


Notes:

<# of processes> is the number of background processes the script will run at a time.
I recommend only using as many as there are cores on your machine.  

You can queue up as many expansion limits as you would like, or split them up among different machines, ie:

On 12-core machine run limits 10, 30, 100:

./backupTestHarness.sh 1 100 AS 12 SlidingPuzzle 4 10 30 100

On 12-core machine run limits 300, 1000:

./backupTestHarness.sh 1 100 AS 12 SlidingPuzzle 4 300 1000

You can also split instances between machines using <starting instance #> and <# of instances to test>, ie:

On 12-core machine run Korf instances 1 through 50:

./backupTestHarness.sh 1 50 AS 12 SlidingPuzzle 4 10 30 100 300 1000

On 12-core machine run Korf instances 51 through 100:

./backupTestHarness.sh 51 50 AS 12 SlidingPuzzle 4 10 30 100 300 1000

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

To run expansion policy tests:

On 1000 random trees of branching factor 2 and depth 100:

./expansionTestHarness.sh 1 1000 <# of processes> TreeWorld 2 100 10 30 100 300 1000


On Korf's 100 puzzle instances:

./expansionTestHarness.sh 1 100 <# of processes> SlidingPuzzle 4 10 30 100 300 1000

Notes:

<# of processes> is the number of background processes the script will run at a time.
I recommend only using as many as there are cores on your machine.  

You can queue up as many expansion limits as you would like, or split them up among different machines, ie:

On 12-core machine run limits 10, 30, 100:

./expansionTestHarness.sh 1 1000 12 TreeWorld 2 100 10 30 100

On 12-core machine run limits 300, 1000:

./expansionTestHarness.sh 1 1000 12 TreeWorld 2 100 300 1000

You can also split instances between machines using <starting instance #> and <# of instances to test>, ie:

On 12-core machine run random tree instances 1 through 500:

./expansionTestHarness.sh 1 500 12 TreeWorld 2 100 10 30 100 300 1000

On 12-core machine run random tree instances 501 through 1000:

./expansionTestHarness.sh 501 500 12 TreeWorld 2 100 10 30 100 300 1000
