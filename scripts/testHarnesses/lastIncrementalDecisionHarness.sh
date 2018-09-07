#!/bin/bash

if (($# < 1))
then
  echo "./lastIncrementalDecisionHarness.sh <# of instances to test>"

  exit 1
fi

# The maximum number of instances to test on
maxInstances=$1

branchFactors="
2"

depths="
10"

numProcs=0

for b in ${branchFactors[@]}
do
  for d in ${depths[@]}
  do
	mkdir ../../results/TreeWorld/lastIncrementalDecision/b${b}d${d}
	instance=0
	testInstancesRun=0
	for file in ../../worlds/treeWorld/b${b}d${d}-*
	do
	  if ((testInstancesRun >= maxInstances))
	  then
	    break
	  fi
	  if ((numProcs >= 12))
	  then
		wait
		numProcs=0
	  fi
	  if [ -f ../../results/TreeWorld/lastIncrementalDecision/b${b}d${d}/LID-b${b}d${d}-${instance}.json ]
	  then 
		let instance++
	  else
		./../../lastIncremental.sh "$((d - 1))" ../../results/TreeWorld/lastIncrementalDecision/b${b}d${d}/LID-b${b}d${d}-${instance}.json < $file &
		let instance++
		let numProcs++
	  fi
	  let testInstancesRun++
	done
  done
done
