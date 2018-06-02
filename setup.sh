#!/bin/bash

# This script is just to set everything up once the repo has been cloned...

cd scripts

chmod a+x *

./make.sh

cd ..

mkdir results
mkdir results/treeWorld
mkdir results/lastIncrementalDecision
mkdir results/backupTests

mkdir worlds
mkdir worlds/treeWorld

./generateTrees.sh 10 2 3000 worlds/treeWorld
./generateTrees.sh 100 2 3000 worlds/treeWorld
./generateTrees.sh 1000 2 3000 worlds/treeWorld