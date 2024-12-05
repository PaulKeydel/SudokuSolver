# SudokuSolver
This C++ program can solve Sudokus by applying human strategies. Just compile the project using the makefile and run it in terminal. A simple text-based user interface is provided in order to enter the quiz. It's also possible to read a quiz from file (see boards in this repo).

9 logical strategies are implemented:
* Hidden singles
* Naked singles
* Hidden pairs
* Naked pairs
* Naked triplets
* Locked candidates in blocks
* X-wings
* XY-wings
* Colored pairs

For further descriptions see https://www.sudoku9x9.com/sudoku_solving_techniques.php

**Python implementation:** An older version of the solving algorithm was written in Python. Use `python3 sudoku.py $<$ file with board $>$` to test this.