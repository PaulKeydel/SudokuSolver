import numpy as np

a = [
    [0, 0, 0,   1, 0, 2,   0, 0, 0],
    [0, 8, 0,   0, 0, 0,   4, 5, 0],
    [0, 0, 0,   0, 0, 9,   0, 0, 0],

    [0, 0, 0,   0, 6, 0,   0, 0, 0],
    [0, 0, 0,   0, 0, 0,   0, 0, 0],
    [0, 0, 0,   0, 7, 0,   0, 0, 0],

    [0, 0, 0,   0, 0, 0,   0, 0, 0],
    [0, 0, 0,   0, 0, 0,   0, 0, 0],
    [0, 0, 0,   0, 0, 0,   0, 0, 0]
    ]
aa = [
    [2, 5, 0,   1, 3, 0,   9, 8, 0],
    [9, 0, 0,   0, 7, 0,   1, 0, 0],
    [8, 0, 7,   0, 0, 5,   4, 0, 6],

    [7, 0, 5,   2, 0, 1,   3, 0, 0],
    [0, 2, 0,   0, 8, 3,   0, 0, 9],
    [0, 4, 8,   6, 0, 0,   0, 0, 5],

    [1, 3, 0,   0, 0, 8,   0, 2, 0],
    [0, 0, 0,   0, 5, 9,   0, 7, 0],
    [0, 0, 6,   4, 0, 0,   8, 0, 3]
    ]

class SudokuBoard:
    def __init__(self, board):
        self.b = board
    
    def at(self, row, col):
        assert(col >= 0 and col < 9)
        assert(row >= 0 and row < 9)
        return self.b[row][col]
    
    def atBlock(self, block, index):
        assert(block >= 0 and block < 9)
        assert(index >= 0 and index < 9)
        row_ = 3 * (block // 3) + (index // 3)
        col_ = 3 * (block % 3) + (index % 3)
        return self.b[row_][col_]
        
    def isInCol(self, col, digit):
        assert(col >= 0 and col < 9)
        i = 0
        found = False
        while (i < 9 and not found):
            found = (self.b[i][col] == digit)
            i = i + 1
        return found

    def isInRow(self, row, digit):
        assert(row >= 0 and row < 9)
        i = 0
        found = False
        while (i < 9 and not found):
            found = (self.b[row][i] == digit)
            i = i + 1
        return found

    def isInBlock(self, block, digit):
        assert(block >= 0 and block < 9)
        i = 0
        found = False
        while (i < 9 and not found):
            found = (self.atBlock(block, i) == digit)
            i = i + 1
        return found
    
    def isInBlockRow(self, block, row, digit):
        assert(block >= 0 and block < 9)
        assert(row >= 0 and row < 3)
        return (self.atBlock(block, 3 * row + 0) == digit) or (self.atBlock(block, 3 * row + 1) == digit) or (self.atBlock(block, 3 * row + 2) == digit)
    
    def isInBlockCol(self, block, col, digit):
        assert(block >= 0 and block < 9)
        assert(col >= 0 and col < 3)
        return (self.atBlock(block, col + 0) == digit) or (self.atBlock(block, col + 3) == digit) or (self.atBlock(block, col + 6) == digit)
    
    def isComplete(self):
        for r in range(9):
            for c in range(9):
                if self.b[r][c] == 0:
                    return False
        return True

    def valid(self):
        if self.isComplete() == False:
            return False
        exist = True
        for dig in range(1, 10):
            for i_ in range(9):
                exist = exist and self.isInBlock(i_, dig) and self.isInCol(i_, dig) and self.isInRow(i_, dig)
        return exist


class SudokuSolver(SudokuBoard):
    def __init__(self, board):
        super().__init__(board)

    def perform_naked_single(self, block, idx):
        if self.atBlock(block, idx) != 0:
            return
        row = 3 * (block // 3) + (idx // 3)
        col = 3 * (block % 3) + (idx % 3)
        num_exist_neighbors = 0
        missing_neighbor = 0
        for dig in range(1, 10):
            if (self.isInRow(row, dig) or self.isInCol(col, dig) or self.isInBlock(block, dig)):
                num_exist_neighbors = num_exist_neighbors + 1
            else:
                missing_neighbor = dig
        if num_exist_neighbors == 8:
            self.b[row][col] = missing_neighbor
    
    def perform_hidden_single(self, block, idx):
        if self.atBlock(block, idx) != 0:
            return
        r = idx // 3
        c = idx % 3
        row = 3 * (block // 3) + r
        col = 3 * (block % 3) + c
        for dig in range(1, 10):
            if self.isInBlock(block, dig):
                continue
            #horizontal looking
            if ((c == 0 and self.atBlock(block, idx + 1) != 0 and self.atBlock(block, idx + 2) != 0) or (c == 1 and self.atBlock(block, idx - 1) != 0 and self.atBlock(block, idx + 1) != 0) or (c == 2 and self.atBlock(block, idx - 2) != 0 and self.atBlock(block, idx - 1) != 0)):
                if (r == 0 and self.isInRow(row + 1, dig) and self.isInRow(row + 2, dig)):
                    self.b[row][col] = dig
                    return
                if (r == 1 and self.isInRow(row - 1, dig) and self.isInRow(row + 1, dig)):
                    self.b[row][col] = dig
                    return
                if (r == 2 and self.isInRow(row - 2, dig) and self.isInRow(row - 1, dig)):
                    self.b[row][col] = dig
                    return
            #vertical looking
            if ((r == 0 and self.atBlock(block, idx + 3) != 0 and self.atBlock(block, idx + 6) != 0) or (r == 1 and self.atBlock(block, idx - 3) != 0 and self.atBlock(block, idx + 3) != 0) or (r == 2 and self.atBlock(block, idx - 6) != 0 and self.atBlock(block, idx - 3) != 0)):
                if (c == 0 and self.isInCol(col + 1, dig) and self.isInCol(col + 2, dig)):
                    self.b[row][col] = dig
                    return
                if (c == 1 and self.isInCol(col - 1, dig) and self.isInCol(col + 1, dig)):
                    self.b[row][col] = dig
                    return
                if (c == 2 and self.isInCol(col - 2, dig) and self.isInCol(col - 1, dig)):
                    self.b[row][col] = dig
                    return
    
    def solve(self):
        valid = False
        while not valid:
            for blk in range(9):
                for idx in range(9):
                    self.perform_naked_single(blk, idx)
                    self.perform_hidden_single(blk, idx)
            valid = self.valid()
    
    def print(self):
        for row in range(9):
            print("{}  {}  {}    {}  {}  {}    {}  {}  {}".format(self.at(row, 0),self.at(row, 1),self.at(row, 2),self.at(row, 3),self.at(row, 4),self.at(row, 5),self.at(row, 6),self.at(row, 7),self.at(row, 8)))
            if (row % 3 == 2):
                print("")


ss = SudokuSolver(aa)
#ss.perform_naked_single(1, 4)
#ss.perform_hidden_single(3, 4)
#ss.perform_hidden_single(7, 6)
#print(ss.atBlock(3, 4))
#print(ss.atBlock(7, 6))
ss.solve()
ss.print()