# ABOUT

This repo contains code for studying real-time heuristic search algorithms. These algorithms can be broken down into three phases: expansion, decision, and learning. The code is this repo is mainly concerned bout exploring the topic of which node is best to expand (by comparing expansion policies) and how best to aggregate information at the search frontier for decision-making (backup rules).

In regards to decision-making and backup rules, we have implementations of minimin, Bellman, Cserna, k-best, and Nancy backups.

As for expansion policies, implementations are of breadth-first search, depth-first search, A*, f^, and risk-based expansion.

Learning policies, such as the reverse Dijkstra method utilized by LSS-LRTA*, are also implemented.

These various policies can be combined to create real-time search algorithms.  For example, A* expansion can be combined with reverse Dijkstra learning and minimin backups to yield LSS-LRTA*.  f^ expansion can be combined with Bellman backups and reverse Dijkstra learning to yield f^, a variation of LSS-LRTA* that expands and makes decisions based on expected value rather than f.  Risk-based expansion can be combined with reverse Dijkstra learning and Nancy backups to yield the Nancy algorithm.

# TO BUILD

cd scripts/

./make.sh

# TO RUN TESTS FOR MY THESIS AND AAAI SUBMISSION

cd scripts/testHarnesses/

## BACKUP RULE TESTS ON THE LAST INCREMENTAL DECISION
./lastIncrementalDecisionHarness.sh 30000

Notes:

This script will perform searches using minimin, Bellman, Nancy, Cserna, and k-Best backups on 30000 instances of a random binary tree of depth 10.
The script will spawn 12 background process at a time to perform the searches.  The 30000 instances are chosen from the world directory, starting with instance 1.

## BACKUP RULE TESTS WITH DEPTH-FIRST EXPANSION POLICY
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

## BACKUP RULE TESTS WITH A* EXPANSION POLICY
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

## EXPANSION POLICY TESTS
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
