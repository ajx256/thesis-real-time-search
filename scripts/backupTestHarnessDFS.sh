#!/bin/bash

if (($# < 2))
then
  echo "./backupTestHarnessDFS.sh <# of processes> <Lookahead value 1> <optional: additional lookahead values>"
  exit 1
fi

# Max number of background processes to start, should probably not be more than the number of cores on the machine
maxProcs=$1

branchFactors="
2"
# Maybe 100, 300, 1000
depths="
100"

# Test on lookahead depths of 3, 7, 10, 30

numProcs=0

for b in ${branchFactors[@]}
do
  for d in ${depths[@]}
  do
    for lookahead in "${@:2}"
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
		  ./../backupTestsDFS.sh ${lookahead} ../results/backupTests/DFS/b${b}d${d}/LA${lookahead}-${instance}.json < ${file} &
		  let instance++
          let numProcs++
		fi
      done
    done
  done
done
