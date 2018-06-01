#!/bin/bash

branchFactors="
2"

depths="
10
100
1000"

lookaheads="
5
10
15
20"

numProcs=0

for b in ${branchFactors[@]}
do
  for d in ${depths[@]}
  do
    for lookahead in ${lookaheads[@]}
    do
      mkdir ../results/backupTests/b${b}d${d}
      instance=0
      for file in ../worlds/treeWorld/b${b}d${d}-*
      do
        if ((numProcs >= 7))
        then
          wait
          numProcs=0
        fi		  
		if [ -f ../results/backupTests/b${b}d${d}/LA${lookahead}-${instance}.json ]
		then 
		  let instance++
		else
		  ./../backupTests.sh ${lookahead} ../results/backupTests/b${b}d${d}/LA${lookahead}-${instance}.json < ${file} &
		  let instance++
          let numProcs++
		fi
      done
    done
  done
done