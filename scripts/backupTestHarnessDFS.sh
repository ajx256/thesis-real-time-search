#!/bin/bash

if (($# < 3))
then
  echo "./backupTestHarnessDFS.sh <# of processes> <Domain Type> <Lookahead value 1> <optional: additional lookahead values>"
  echo "Available domain types are TreeWorld and SlidingPuzzle"
  exit 1
fi

# Max number of background processes to start, should probably not be more than the number of cores on the machine
maxProcs=$1

domainType=$2

branchFactors="
2"
# Maybe 100, 300, 1000
depths="
100"

# Test on lookahead depths of 3, 7, 10, 30

numProcs=0

if [ "$domainType" = "TreeWorld" ]
then
  for b in ${branchFactors[@]}
  do
    for d in ${depths[@]}
    do
      for lookahead in "${@:3}"
      do
        mkdir ../results/backupTests/DFS/b${b}d${d}
        instance=0
        for file in ../worlds/treeWorld/b${b}d${d}-*
        do
          if ((numProcs >= ${maxProcs}))
          then
            wait
            numProcs=0
          fi		  
		  if [ -f ../results/backupTests/DFS/b${b}d${d}/LA${lookahead}-${instance}.json ]
		  then 
		    let instance++
		  else
		    ./../backupTestsDFS.sh ${domainType} ${lookahead} ../results/backupTests/DFS/b${b}d${d}/LA${lookahead}-${instance}.json < ${file} &
		    let instance++
            let numProcs++
		  fi
        done
      done
    done
  done
elif [ "$domainType" = "SlidingPuzzle" ]
then
  for lookahead in "${@:3}"
  do
    mkdir ../results/backupTests/DFS/SlidingTilePuzzle
    instance=0
    for file in ../worlds/slidingTile/*
    do
      if ((numProcs >= ${maxProcs}))
      then
        wait
        numProcs=0
      fi		  
	  if [ -f ../results/backupTests/DFS/SlidingTilePuzzle/LA${lookahead}-${instance}.json ]
	  then 
	    let instance++
	  else
	    ./../backupTestsDFS.sh ${domainType} ${lookahead} ../results/backupTests/DFS/SlidingTilePuzzle/LA${lookahead}-${instance}.json < ${file} &
	    let instance++
        let numProcs++
	  fi
    done
  done
else
  echo "Available domain types are TreeWorld and SlidingPuzzle"
  exit 1
fi