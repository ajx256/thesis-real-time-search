#!/bin/bash

if (($# < 6))
then
  echo "./backupTestHarness.sh <starting instance #> <# of instances to test> <Lookahead Type> <# of processes> <Domain Type> <Domain Variables> <Lookahead value 1> <optional: additional lookahead values>"
  echo "Available lookahead types are AS and DFS"
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

# The type of lookahead to use
laType=$3

# Max number of background processes to start, should probably not be more than the number of cores on the machine
maxProcs=$4

# The domain to run on
domainType=$5

numProcs=0
if [ "$laType" = "AS" ]
then
  if [ "$domainType" = "TreeWorld" ]
  then
    branchFactors=$6
	depths=$7
    for b in ${branchFactors[@]}
    do
      for d in ${depths[@]}
      do
        for lookahead in "${@:8}"
        do
          mkdir ../../results/TreeWorld/backupTests/AS/b${b}d${d}
          instance=$firstInstance
          while ((instance < lastInstance))
          do
		    file="../../worlds/treeWorld/b${b}d${d}-${instance}.tw"
            if ((numProcs >= ${maxProcs}))
            then
              wait
              numProcs=0
            fi		  
		    if [ -f ../../results/TreeWorld/backupTests/AS/b${b}d${d}/LA${lookahead}-${instance}.json ]
		    then 
		      let instance++
		    else
		      ./../../backupTestsAS.sh ${domainType} ${lookahead} ../../results/TreeWorld/backupTests/AS/b${b}d${d}/LA${lookahead}-${instance}.json < ${file} &
		      let instance++
              let numProcs++
		    fi
          done
        done
      done
    done
  elif [ "$domainType" = "SlidingPuzzle" ]
  then
    dimensions=$6
    for lookahead in "${@:7}"
    do
      mkdir ../../results/SlidingTilePuzzle/backupTests/AS/${dimensions}x${dimensions}
      instance=$firstInstance
	  while ((instance < lastInstance))
      do
	    file="../../worlds/slidingTile/${instance}-${dimensions}x${dimensions}.st"
        if ((numProcs >= ${maxProcs}))
        then
          wait
          numProcs=0
        fi		  
	    if [ -f ../../results/SlidingTilePuzzle/backupTests/AS/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json ]
	    then 
	      let instance++
	    else
	      ./../../backupTestsAS.sh ${domainType} ${lookahead} ../../results/SlidingTilePuzzle/backupTests/AS/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json < ${file} &
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
elif [ "$laType" = "DFS" ]
then
  if [ "$domainType" = "TreeWorld" ]
  then
    branchFactors=$6
	depths=$7
    for b in ${branchFactors[@]}
    do
      for d in ${depths[@]}
      do
        for lookahead in "${@:8}"
        do
          mkdir ../../results/TreeWorld/backupTests/DFS/b${b}d${d}
          instance=$firstInstance
          while ((instance < lastInstance))
          do
		    file="../../worlds/treeWorld/b${b}d${d}-${instance}.tw"
            if ((numProcs >= ${maxProcs}))
            then
              wait
              numProcs=0
            fi		  
		    if [ -f ../../results/TreeWorld/backupTests/DFS/b${b}d${d}/LA${lookahead}-${instance}.json ]
		    then 
		      let instance++
		    else
		      ./../../backupTestsDFS.sh ${domainType} ${lookahead} ../../results/TreeWorld/backupTests/DFS/b${b}d${d}/LA${lookahead}-${instance}.json < ${file} &
		      let instance++
              let numProcs++
		    fi
          done
        done
      done
    done
  elif [ "$domainType" = "SlidingPuzzle" ]
  then
    dimensions=$6
    for lookahead in "${@:7}"
    do
      mkdir ../../results/SlidingTilePuzzle/backupTests/DFS/${dimensions}x${dimensions}
      instance=$firstInstance
	  while ((instance < lastInstance))
      do
	    file="../../worlds/slidingTile/${instance}-${dimensions}x${dimensions}.st"
        if ((numProcs >= ${maxProcs}))
        then
          wait
          numProcs=0
        fi		  
	    if [ -f ../../results/SlidingTilePuzzle/backupTests/DFS/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json ]
	    then 
	      let instance++
	    else
	      ./../../backupTestsDFS.sh ${domainType} ${lookahead} ../../results/SlidingTilePuzzle/backupTests/DFS/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json < ${file} &
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
else
  echo "Available lookahead types are AS and DFS"
  exit 1
fi
