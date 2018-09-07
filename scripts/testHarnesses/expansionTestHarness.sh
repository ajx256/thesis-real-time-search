#!/bin/bash

if (($# < 6))
then
  echo "./expansionTestHarness.sh <starting instance #> <# of instances to test> <# of processes> <Domain Type> <Domain Variables> <Lookahead value 1> <optional: additional lookahead values>"
  echo "Available domain types are TreeWorld and SlidingPuzzle"
  echo "Domain variables for TreeWorld: <branching factor> <tree depth>"
  echo "Domain variables for SlidingPuzzle: <puzzle dimensions>"
  exit 1
fi

# Which instance to start testing on
firstInstance=$1

# The maximum number of instances to test on
maxInstances=$2
lastInstance=$(( $firstInstance + $maxInstances ))

# Max number of background processes to start, should probably not be more than the number of cores on the machine
maxProcs=$3

# The domain to run on
domainType=$4

numProcs=0

if [ "$domainType" = "TreeWorld" ]
then
  branchFactors=$5
  depths=$6
  for b in ${branchFactors[@]}
  do
    for d in ${depths[@]}
    do
      for lookahead in "${@:7}"
      do
        mkdir ../../results/TreeWorld/expansionTests/Nancy/b${b}d${d}
        instance=$firstInstance
        while ((instance < lastInstance))
        do
		  file="../../worlds/treeWorld/b${b}d${d}-${instance}.tw"
          if ((numProcs >= ${maxProcs}))
          then
            wait
            numProcs=0
          fi		  
          if [ -f ../../results/TreeWorld/expansionTests/Nancy/b${b}d${d}/LA${lookahead}-${instance}.json ]
	      then 
	        let instance++
	      else
	        ./../../expansionTests.sh ${domainType} ${lookahead} ../../results/TreeWorld/expansionTests/Nancy/b${b}d${d}/LA${lookahead}-${instance}.json < ${file} &
	        let instance++
            let numProcs++
	      fi
        done
      done
    done
  done
elif [ "$domainType" = "SlidingPuzzle" ]
then
  dimensions=$5
  for lookahead in "${@:6}"
  do
    mkdir ../../results/SlidingTilePuzzle/expansionTests/Nancy/${dimensions}x${dimensions}
    instance=$firstInstance
    while ((instance < lastInstance))
    do
	  file="../../worlds/slidingTile/${instance}-${dimensions}x${dimensions}.st"
      if ((numProcs >= ${maxProcs}))
      then
        wait
        numProcs=0
      fi		  
      if [ -f ../../results/SlidingTilePuzzle/expansionTests/Nancy/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json ]
	  then 
	    let instance++
	  else
	    ./../../expansionTests.sh ${domainType} ${lookahead} ../../results/SlidingTilePuzzle/expansionTests/Nancy/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json < ${file} &
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