# SudokuSolver
This C++ program can solve Sudokus by applying human strategies. A simple text-based user interface is provided in order to easily enter the quiz. It's also possible to read a quiz from file (see boards in this repo).

**How does the solving algorithm work?:** First, the program passes through all empty cells and collects all possible candidates for each cell. Depending on how many entries are given in the same row, column or block, an empty cell can have one or more possible candidates. Based on logical rules and dependencies between cells, the algortihm then tries to eliminate candidates until only one number is left in the set. 9 logical strategies are implemented so far:

* Hidden singles
* Naked singles
* Hidden pairs
* Naked pairs
* Naked triplets
* Locked candidates in blocks
* X-wings
* XY-wings
* Colored pairs

For further descriptions and illustrations see https://www.sudoku9x9.com/sudoku_solving_techniques.php

**How to use the algorithm?:** Just compile the project using the makefile (run `make` or `make debug`) and run the executable in terminal. Please note that the makefile is configured to use the gcc compiler. Change this if you're on a non-UNIX system.

As an example you might solve the quiz `test.txt` from the testboard directory. Then simply run `./sudoku testboards/test.txt`. The program will solve the quiz and prints all solving steps:
```
4  6  9    2  3  5    8  1  7    
2  5  1    7  8  9    6  4  3    
3  7  8    1  6  4    5  9  2    

6  2  3    9  4  1    7  5  8    
5  9  4    3  7  8    2  6  1    
8  1  7    5  2  6    4  3  9    

1  4  5    8  9  7    3  2  6    
9  8  2    6  5  3    1  7  4    
7  3  6    4  1  2    9  8  5    

(0, 1): Cands {6} are locked in block
(0, 4): Cands {3, 6} are locked in col
(0, 6): Cands {8} are locked in col
(2, 7): Hidden Single
(2, 8): Naked Triplet in col with cell (7, 8) and cell (8, 8)
(3, 4): Cands {1} are locked in row
(3, 6): Cands {2} are locked in col
(3, 8): Naked Single
(4, 0): Cands {8} are locked in block
(4, 3): row-wise X-Wing with diag cell(5, 7)
(4, 5): Hidden Single
(4, 7): Cands {3} are locked in block
(5, 0): Hidden Single
(5, 3): Hidden Pair {3, 5} with cell (4, 3)
(6, 6): Hidden Single
(7, 1): Hidden Single
(7, 2): Hidden Single
(7, 6): Cands {7} are locked in block
(7, 7): Naked Triplet in block with cell (7, 8) and cell (8, 8)
(7, 8): Naked Pair with cell (8, 8)
(8, 2): Hidden Single
(8, 3): Hidden Pair {4, 7} with cell (1, 3)
(8, 5): Hidden Single
(8, 6): Hidden Single
(8, 8): Naked Pair with cell (7, 8)
(0, 0): Cands {3} are locked in block
(0, 5): Naked Triplet in block with cell (1, 3) and cell (2, 5)
(1, 0): Cands {1, 2, 5} are locked in block
(1, 3): Naked Pair with cell (2, 5)
(1, 4): Naked Triplet in row with cell (1, 6) and cell (1, 7)
(1, 6): Cands {4} are locked in block
(2, 5): Hidden Single
(2, 8): Naked Single
(3, 2): Hidden Single
(3, 4): Hidden Single
(3, 5): Naked Single
(3, 6): Cands {2, 7} are locked in block
(6, 0): Cands {4} are locked in block
(6, 4): Hidden Single
(6, 5): Hidden Single
(7, 4): row-wise X-Wing with diag cell(8, 8)
(7, 6): Naked Single
(7, 7): Naked Single
(7, 8): Hidden Single
(8, 0): Hidden Single
(8, 3): Naked Single
(8, 4): Hidden Single
(8, 8): Naked Single
(0, 5): Naked Single
(1, 3): Naked Single
(2, 0): Naked Single
(2, 1): Hidden Single
(2, 4): Naked Single
(3, 1): Naked Single
(3, 6): Naked Single
(4, 0): Naked Triplet in row with cell (4, 1) and cell (4, 2)
(4, 1): Cands {4, 5, 9} are locked in block
(4, 3): Naked Single
(4, 6): Hidden Single
(4, 7): Naked Single
(5, 1): Naked Single
(5, 2): Naked Single
(5, 3): Naked Single
(5, 6): Naked Single
(5, 7): Naked Single
(7, 4): Naked Single
(0, 0): Naked Single
(0, 1): Hidden Single
(0, 2): Naked Single
(0, 4): Hidden Single
(0, 6): Naked Single
(1, 0): Hidden Single
(1, 1): Naked Single
(1, 2): Naked Single
(1, 4): Naked Single
(1, 6): Naked Single
(1, 7): Naked Single
(4, 0): Naked Single
(4, 1): Hidden Single
(4, 2): Naked Single
(6, 0): Naked Single
(6, 1): Naked Single
(6, 2): Naked Single
Solved
```
**Documentation:** The repository additionally contains a Doxygen config file. If you have Doxygen installed, run `make doc` to create the documentation. Otherwise you'll find all relevant comments in `solver.h`.

**Python implementation:** An older version of the solving algorithm was written in Python. Use `python3 sudoku.py <file with board>` to test this.