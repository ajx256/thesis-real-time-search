#!/bin/bash

for i in {10..1000}
do
    ./../../generateTrees $i 2 100 ../../worlds/treeWorld
done