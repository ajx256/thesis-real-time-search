#!/bin/bash

if (($# < 4))
then
  echo "./expansionTestHarness.sh <# of processes> <Domain Type> <Domain Variables> <Lookahead value 1> <optional: additional lookahead values>"
  echo "Available domain types are TreeWorld and SlidingPuzzle"
  echo "Domain variables for TreeWorld: <branching factor> <tree depth>"
  echo "Domain variables for SlidingPuzzle: <puzzle dimensions>"
  exit 1
fi

# Max number of background processes to start, should probably not be more than the number of cores on the machine
maxProcs=$1

# The domain to run on
domainType=$2

numProcs=0

if [ "$domainType" = "TreeWorld" ]
then
  branchFactors=$3
  depths=$4
  for b in ${branchFactors[@]}
  do
    for d in ${depths[@]}
    do
      for lookahead in "${@:5}"
      do
        mkdir ../results/TreeWorld/expansionTests/Nancy/b${b}d${d}
        instance=0
        for file in ../worlds/treeWorld/b${b}d${d}-*
        do
          if ((numProcs >= ${maxProcs}))
          then
            wait
            numProcs=0
          fi		  
          if [ -f ../results/TreeWorld/expansionTests/Nancy/b${b}d${d}/LA${lookahead}-${instance}.json ]
	      then 
	        let instance++
	      else
	        ./../expansionTests.sh ${domainType} ${lookahead} ../results/TreeWorld/expansionTests/Nancy/b${b}d${d}/LA${lookahead}-${instance}.json < ${file} &
	        let instance++
            let numProcs++
	      fi
        done
      done
    done
  done
elif [ "$domainType" = "SlidingPuzzle" ]
then
  dimensions=$3
  for lookahead in "${@:4}"
  do
    mkdir ../results/SlidingTilePuzzle/expansionTests/Nancy/${dimensions}x${dimensions}
    instance=0
    for file in ../worlds/slidingTile/*
    do
      if ((numProcs >= ${maxProcs}))
      then
        wait
        numProcs=0
      fi		  
      if [ -f ../results/SlidingTilePuzzle/expansionTests/Nancy/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json ]
	  then 
	    let instance++
	  else
	    ./../expansionTests.sh ${domainType} ${lookahead} ../results/SlidingTilePuzzle/expansionTests/Nancy/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json < ${file} &
	    let instance++
        let numProcs++
	  fi
    done
  done
else
  echo "Available domain types are TreeWorld and SlidingPuzzle"
  echo "Domain variables for TreeWorld: <branching factor> <tree depth>"
  echo "Domain variables for SlidingPuzzle: <puzzle dimensions>"
  exit 1
fi