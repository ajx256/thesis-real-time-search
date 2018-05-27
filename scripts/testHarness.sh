#!/bin/bash

branchFactors="
2"

depths="
10
100
1000"

lookaheads="
10
100
1000
10000"

algorithms="
a-star
a-star-f-hat
beam-10
beam-100
beam-1000
k-best-1
k-best-3
k-best-5
k-best-7
nancy"

numProcs=0

for b in ${branchFactors[@]}
do
  for d in ${depths[@]}
  do
    for algorithm in ${algorithms[@]}
    do
      for lookahead in ${lookaheads[@]}
      do
        mkdir ../results/treeWorld/b${b}d${d}
		instance=0
        for file in ../worlds/treeWorld/b${b}d${d}-*
        do
          if ((numProcs >= 7))
          then
            wait
            numProcs=0
          fi		  
		  # Only do 500 instances
		  if ((instance >= 499))
		  then
		    break
		  fi
		  if [ -f ../results/treeWorld/b${b}d${d}/LA${lookahead}-${algorithm}-${instance}.json ]
		  then 
		    let instance++
		  else
            if [ "$algorithm" == "beam-10" ]
            then
              ./../run.sh beam ${lookahead} 10 ../results/treeWorld/b${b}d${d}/LA${lookahead}-${algorithm}-${instance}.json < $file &
		    elif [ "$algorithm" == "beam-100" ]
		    then
		      ./../run.sh beam ${lookahead} 100 ../results/treeWorld/b${b}d${d}/LA${lookahead}-${algorithm}-${instance}.json < $file &
		    elif [ "$algorithm" == "beam-1000" ]
		    then
		      ./../run.sh beam ${lookahead} 1000 ../results/treeWorld/b${b}d${d}/LA${lookahead}-${algorithm}-${instance}.json < $file &
			elif [ "$algorithm" == "k-best-1" ]
            then
              ./../run.sh k-best ${lookahead} 1 ../results/treeWorld/b${b}d${d}/LA${lookahead}-${algorithm}-${instance}.json < $file &
            elif [ "$algorithm" == "k-best-3" ]
            then
              ./../run.sh k-best ${lookahead} 3 ../results/treeWorld/b${b}d${d}/LA${lookahead}-${algorithm}-${instance}.json < $file &
		    elif [ "$algorithm" == "k-best-5" ]
		    then
		      ./../run.sh k-best ${lookahead} 5 ../results/treeWorld/b${b}d${d}/LA${lookahead}-${algorithm}-${instance}.json < $file &
		    elif [ "$algorithm" == "k-best-7" ]
		    then
		      ./../run.sh k-best ${lookahead} 7 ../results/treeWorld/b${b}d${d}/LA${lookahead}-${algorithm}-${instance}.json < $file &
            else
		      ./../run.sh ${algorithm} ${lookahead} ../results/treeWorld/b${b}d${d}/LA${lookahead}-${algorithm}-${instance}.json < $file &
            fi
		    let instance++
			let numProcs++
		  fi
        done
      done
    done
  done
done
