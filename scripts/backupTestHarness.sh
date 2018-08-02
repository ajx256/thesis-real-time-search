#!/bin/bash

if (($# < 6))
then
  echo "./backupTestHarness.sh <# of instances to test> <Lookahead Type> <# of processes> <Domain Type> <Domain Variables> <Lookahead value 1> <optional: additional lookahead values>"
  echo "Available lookahead types are AS and DFS"
  echo "Available domain types are TreeWorld and SlidingPuzzle"
  echo "Domain variables for TreeWorld: <branching factor> <tree depth>"
  echo "Domain variables for SlidingPuzzle: <puzzle dimensions>"
  exit 1
fi

# The maximum number of instances to test on
maxInstances=$1

# The type of lookahead to use
laType=$2

# Max number of background processes to start, should probably not be more than the number of cores on the machine
maxProcs=$3

# The domain to run on
domainType=$4

numProcs=0
if [ "$laType" = "AS" ]
then
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
          mkdir ../results/TreeWorld/backupTests/AS/b${b}d${d}
          instance=0
		  testInstancesRun=0
          for file in ../worlds/treeWorld/b${b}d${d}-*
          do
		    if ((testInstancesRun >= maxInstances))
			then
			  break
			fi
            if ((numProcs >= ${maxProcs}))
            then
              wait
              numProcs=0
            fi		  
		    if [ -f ../results/TreeWorld/backupTests/AS/b${b}d${d}/LA${lookahead}-${instance}.json ]
		    then 
		      let instance++
		    else
		      ./../backupTestsAS.sh ${domainType} ${lookahead} ../results/TreeWorld/backupTests/AS/b${b}d${d}/LA${lookahead}-${instance}.json < ${file} &
		      let instance++
              let numProcs++
		    fi
			let testInstancesRun++
          done
        done
      done
    done
  elif [ "$domainType" = "SlidingPuzzle" ]
  then
    dimensions=$5
    for lookahead in "${@:6}"
    do
      mkdir ../results/SlidingTilePuzzle/backupTests/AS/${dimensions}x${dimensions}
      instance=0
	  testInstancesRun=0
      for file in ../worlds/slidingTile/*
      do
	  	if ((testInstancesRun >= maxInstances))
		then
		  break
		fi
        if ((numProcs >= ${maxProcs}))
        then
          wait
          numProcs=0
        fi		  
	    if [ -f ../results/SlidingTilePuzzle/backupTests/AS/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json ]
	    then 
	      let instance++
	    else
	      ./../backupTestsAS.sh ${domainType} ${lookahead} ../results/SlidingTilePuzzle/backupTests/AS/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json < ${file} &
	      let instance++
          let numProcs++
	    fi
		let testInstancesRun++
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
    branchFactors=$5
	depths=$6
    for b in ${branchFactors[@]}
    do
      for d in ${depths[@]}
      do
        for lookahead in "${@:7}"
        do
          mkdir ../results/TreeWorld/backupTests/DFS/b${b}d${d}
          instance=0
		  testInstancesRun=0
          for file in ../worlds/treeWorld/b${b}d${d}-*
          do
		  	if ((testInstancesRun >= maxInstances))
			then
			  break
			fi
            if ((numProcs >= ${maxProcs}))
            then
              wait
              numProcs=0
            fi		  
		    if [ -f ../results/TreeWorld/backupTests/DFS/b${b}d${d}/LA${lookahead}-${instance}.json ]
		    then 
		      let instance++
		    else
		      ./../backupTestsDFS.sh ${domainType} ${lookahead} ../results/TreeWorld/backupTests/DFS/b${b}d${d}/LA${lookahead}-${instance}.json < ${file} &
		      let instance++
              let numProcs++
		    fi
			let testInstancesRun++
          done
        done
      done
    done
  elif [ "$domainType" = "SlidingPuzzle" ]
  then
    dimensions=$5
    for lookahead in "${@:6}"
    do
      mkdir ../results/SlidingTilePuzzle/backupTests/DFS/${dimensions}x${dimensions}
      instance=0
	  testInstancesRun=0
      for file in ../worlds/slidingTile/*
      do
	  	if ((testInstancesRun >= maxInstances))
		then
		  break
		fi
        if ((numProcs >= ${maxProcs}))
        then
          wait
          numProcs=0
        fi		  
	    if [ -f ../results/SlidingTilePuzzle/backupTests/DFS/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json ]
	    then 
	      let instance++
	    else
	      ./../backupTestsDFS.sh ${domainType} ${lookahead} ../results/SlidingTilePuzzle/backupTests/DFS/${dimensions}x${dimensions}/LA${lookahead}-${instance}.json < ${file} &
	      let instance++
          let numProcs++
	    fi
		let testInstancesRun++
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
