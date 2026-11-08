#!/bin/bash

cd solver

make

for filename in ../testboards/*.txt; do
    (./sudoku "$filename") > /dev/null
    if [ $? -eq 0 ]
    then
        echo "${filename} ... check"
    else
        echo "${filename} ... cannot be solved"
    fi
done

cd ..