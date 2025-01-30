#!/bin/bash

make

cd testboards
for filename in *.txt; do
    ( echo n | ../sudoku "$filename" ) > /dev/null
    if [ $? -eq 0 ]
    then
        echo "${filename} ... check"
    else
        echo "${filename} ... cannot be solved"
    fi
done
cd ..