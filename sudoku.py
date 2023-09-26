import numpy as np

#cell[9 * 1 + 1]: hidden single
#cell[9 * 4 + 4]: naked single
#cell[9 * 6 + 8] and cell[9 * 8 + 8]: hidden pair (5, 7)
#cell[9 * 6 + 8] and cell[9 * 8 + 8]: naked pair (5, 7), check this with cell[9 * 0 + 8]
testboard1 = [
    [0, 0, 0,   0, 0, 0,   2, 0, 0],
    [1, 0, 3,   0, 0, 0,   0, 0, 0],
    [0, 0, 0,   0, 2, 0,   0, 0, 0],

    [0, 0, 0,   0, 1, 0,   0, 0, 0],
    [0, 0, 4,   0, 0, 0,   0, 8, 9],
    [0, 0, 0,   5, 0, 7,   0, 0, 0],

    [0, 0, 0,   0, 0, 0,   4, 1, 0],
    [7, 0, 0,   2, 3, 5,   0, 0, 0],
    [0, 0, 0,   0, 0, 0,   2, 3, 0]
    ]
testboard2 = [
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

class Cell:
    def __init__(self, idx: int, digit: int):
        #digit of the cell
        self.val = digit
        #position parameters
        self.row = idx // 3
        self.col = idx % 3
        self.blk = 3 * (self.row // 3) + (self.col // 3)
        self.blkidx = 3 * (self.row % 3) + (self.col % 3)
        #set for storing candidates
        self.candidates = set()
    
    def isEq(self, dig):
        return (self.val == dig)
    
    def isGap(self):
        return (self.val == 0)


class SudokuBoard:
    def __init__(self, board):
        self.b = [Cell(i, board[i // 9][i % 9]) for i in range(81)]
    
    def at(self, row, col):
        assert(col >= 0 and col < 9)
        assert(row >= 0 and row < 9)
        return self.b[9 * row + col].val
    
    def atBlock(self, block, index):
        assert(block >= 0 and block < 9)
        assert(index >= 0 and index < 9)
        row_ = 3 * (block // 3) + (index // 3)
        col_ = 3 * (block % 3) + (index % 3)
        return self.b[9 * row_ + col_].val
        
    def isInCol(self, col, digit):
        assert(col >= 0 and col < 9)
        i = 0
        found = False
        while (i < 9 and not found):
            found = self.b[9 * i + col].isEq(digit)
            i = i + 1
        return found

    def isInRow(self, row, digit):
        assert(row >= 0 and row < 9)
        i = 0
        found = False
        while (i < 9 and not found):
            found = self.b[9 * row + i].isEq(digit)
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

    def isPresent(self, row, col, digit):
        block = 3 * (row // 3) + (col // 3)
        return (self.isInRow(row, digit) or self.isInCol(col, digit) or self.isInBlock(block, digit))

    def isComplete(self):
        for i in range(81):
            if self.b[i].val == 0:
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

    def print(self):
        for row in range(9):
            print("{}  {}  {}    {}  {}  {}    {}  {}  {}".format(self.at(row, 0),self.at(row, 1),self.at(row, 2),self.at(row, 3),self.at(row, 4),self.at(row, 5),self.at(row, 6),self.at(row, 7),self.at(row, 8)))
            if (row % 3 == 2):
                print("")
    
    def updateCands(self):
        for idx in range(81):
            if not self.b[idx].isGap():
                continue
            for dig in range(1, 10):
                if not self.isPresent(idx // 9, idx % 9, dig):
                    self.b[idx].candidates.add(dig)
        return
    
    def checkCellForNakedSingle(self, row, col):
        if (len(self.b[9 * row + col].candidates) != 1):
            return
        dig = self.b[9 * row + col].candidates.pop()
        self.b[9 * row + col].val = dig
        #update cand list in same row
        for c in range(9):
            if (c == col):
                continue
            self.b[9 * row + c].candidates.discard(dig)
        #update cand list in same col
        for r in range(9):
            if (r == row):
                continue
            self.b[9 * r + col].candidates.discard(dig)
        #update cand list in same block
        rowstart = 3 * (row // 3)
        colstart = 3 * (col // 3)
        for r in range(rowstart, rowstart + 3):
            for c in range(colstart, colstart + 3):
                if (r == row and c == col):
                    continue
                self.b[9 * r + c].candidates.discard(dig)
        return
    
    def checkCellForHiddenSingle(self, row, col):
        if (not self.b[9 * row + col].isGap()):
            return
        #search along row and collect all other candidates
        allOtherCands = set()
        for c in range(9):
            if (c == col or not self.b[9 * row + c].isGap()):
                continue
            allOtherCands = allOtherCands.union(self.b[9 * row + c].candidates)
        t = self.b[9 * row + col].candidates.difference(allOtherCands)
        if (len(t) == 1):
            self.b[9 * row + col].candidates = t
            dig = self.b[9 * row + col].candidates.pop()
            self.b[9 * row + col].val = dig
        #search along col and collect all other candidates
        allOtherCands = set()
        for r in range(9):
            if (r == row or not self.b[9 * r + col].isGap()):
                continue
            allOtherCands = allOtherCands.union(self.b[9 * r + col].candidates)
        t = self.b[9 * row + col].candidates.difference(allOtherCands)
        if (len(t) == 1):
            self.b[9 * row + col].candidates = t
            dig = self.b[9 * row + col].candidates.pop()
            self.b[9 * row + col].val = dig
        #search within block and collect all other candidates
        allOtherCands = set()
        rowstart = 3 * (row // 3)
        colstart = 3 * (col // 3)
        for r in range(rowstart, rowstart + 3):
            for c in range(colstart, colstart + 3):
                if ((r == row and c == col) or not self.b[9 * r + c].isGap()):
                    continue
                allOtherCands = allOtherCands.union(self.b[9 * r + c].candidates)
        t = self.b[9 * row + col].candidates.difference(allOtherCands)
        if (len(t) == 1):
            self.b[9 * row + col].candidates = t
            dig = self.b[9 * row + col].candidates.pop()
            self.b[9 * row + col].val = dig
        return
    
    def searchForCandSingles(self):
        #look for naked singles and update cand list
        for row in range(9):
            for col in range(9):
                self.checkCellForNakedSingle(row, col)
        #look for hidden singles
        for row in range(9):
            for col in range(9):
                self.checkCellForHiddenSingle(row, col)
        return
    
    def checkCellForNakedPair(self, row, col):
        if (len(self.b[9 * row + col].candidates) != 2):
            return
        candPair = self.b[9 * row + col].candidates
        #update cand list in same row
        colpair = -1
        for c in range(9):
            if (c == col):
                continue
            if (self.b[9 * row + c].candidates == candPair):
                colpair = c
        if (colpair >= 0):
            for c in range(9):
                if ((c == col) or (c == colpair)):
                    continue
                self.b[9 * row + c].candidates.difference_update(candPair)
        #update cand list in same col
        rowpair = -1
        for r in range(9):
            if (r == row):
                continue
            if (self.b[9 * r + col].candidates == candPair):
                rowpair = r
        if (rowpair >= 0):
            for r in range(9):
                if ((r == row) or (r == rowpair)):
                    continue
                self.b[9 * r + col].candidates.difference_update(candPair)
        #update cand list in same block
        rowpair = -1
        colpair = -1
        rowstart = 3 * (row // 3)
        colstart = 3 * (col // 3)
        for r in range(rowstart, rowstart + 3):
            for c in range(colstart, colstart + 3):
                if (r == row and c == col):
                    continue
                if (self.b[9 * r + c].candidates == candPair):
                    rowpair = r
                    colpair = c
        if (rowpair >= 0 and colpair >= 0):
            for r in range(rowstart, rowstart + 3):
                for c in range(colstart, colstart + 3):
                    if ((r == row and c == col) or (r == rowpair and c == colpair)):
                        continue
                    self.b[9 * r + c].candidates.difference_update(candPair)
        return
    
    def checkCellForHiddenPair(self, row, col):
        if (not self.b[9 * row + col].isGap()):
            return
        #search along row
        for c in range(9):
            if (c == col or not self.b[9 * row + c].isGap()):
                continue
            #collect all other candidates
            allOtherCands = set()
            for j in range(9):
                if (j == col or j == c):
                    continue
                allOtherCands = allOtherCands.union(self.b[9 * row + j].candidates)
            t = self.b[9 * row + col].candidates.intersection(self.b[9 * row + c].candidates)
            t = t.difference(allOtherCands)
            if (len(t) == 2):
                self.b[9 * row + col].candidates = t
                self.b[9 * row + c].candidates = t
        #search along col
        for r in range(9):
            if (r == row or not self.b[9 * r + col].isGap()):
                continue
            #collect all other candidates
            allOtherCands = set()
            for i in range(9):
                if (i == row or i == r):
                    continue
                allOtherCands = allOtherCands.union(self.b[9 * i + col].candidates)
            t = self.b[9 * row + col].candidates.intersection(self.b[9 * r + col].candidates)
            t = t.difference(allOtherCands)
            if (len(t) == 2):
                self.b[9 * row + col].candidates = t
                self.b[9 * r + col].candidates = t
        #search within block
        rowstart = 3 * (row // 3)
        colstart = 3 * (col // 3)
        for r in range(rowstart, rowstart + 3):
            for c in range(colstart, colstart + 3):
                if ((r == row and c == col) or not self.b[9 * r + c].isGap()):
                    continue
                #collect all other candidates
                allOtherCands = set()
                for i in range(rowstart, rowstart + 3):
                    for j in range(colstart, colstart + 3):
                        if ((i == row and j == col) or (i == r and j == c)):
                            continue
                        allOtherCands = allOtherCands.union(self.b[9 * i + j].candidates)
                t = self.b[9 * row + col].candidates.intersection(self.b[9 * r + c].candidates)
                t = t.difference(allOtherCands)
                if (len(t) == 2):
                    self.b[9 * row + col].candidates = t
                    self.b[9 * r + c].candidates = t
        return
    
    def searchForCandPairs(self):
        #look for naked pairs and update cand list
        for row in range(9):
            for col in range(9):
                self.checkCellForNakedPair(row, col)
        #look for hidden pairs
        for row in range(9):
            for col in range(9):
                self.checkCellForHiddenPair(row, col)
        return
    
    def searchForLockedCandsInBlocks(self):
        #locked cand: union over cols/rows has excatly one element (at least two gaps)
        #check: this cand is not in the cand list of all other cells
        for rowstart in range(0, 9, 3):
            for colstart in range(0, 9, 3):
                #go through all rows in block
                for r in range(rowstart, rowstart + 3):
                    numGaps = 0
                    lockedCands = set()
                    for c in range(colstart, colstart + 3):
                        if self.b[9 * r + c].isGap():
                            numGaps = numGaps + 1
                            lockedCands = lockedCands.union(self.b[9 * r + c].candidates)
                    if (numGaps > 1):
                        #check if lockedCands is not part of other cand lists
                        allOtherCands = set()
                        for i in range(rowstart, rowstart + 3):
                            for j in range(colstart, colstart + 3):
                                if (i == r or not self.b[9 * i + j].isGap()):
                                    continue
                                allOtherCands = allOtherCands.union(self.b[9 * i + j].candidates)
                        t = lockedCands.difference(allOtherCands)
                        if (len(t) == 1):
                            dig = t.pop()
                            for j in range(9):
                                if ((j >= colstart) and (j < colstart + 3)):
                                    continue
                                self.b[9 * r + j].candidates.discard(dig)
                #go through all cols in block
                for c in range(colstart, colstart + 3):
                    numGaps = 0
                    lockedCands = set()
                    for r in range(rowstart, rowstart + 3):
                        if self.b[9 * r + c].isGap():
                            numGaps = numGaps + 1
                            lockedCands = lockedCands.union(self.b[9 * r + c].candidates)
                    if (numGaps > 1):
                        #check if lockedCands is not part of other cand lists
                        allOtherCands = set()
                        for i in range(rowstart, rowstart + 3):
                            for j in range(colstart, colstart + 3):
                                if (j == c or not self.b[9 * i + j].isGap()):
                                    continue
                                allOtherCands = allOtherCands.union(self.b[9 * i + j].candidates)
                        t = lockedCands.difference(allOtherCands)
                        if (len(t) == 1):
                            dig = t.pop()
                            for i in range(9):
                                if ((i >= rowstart) and (i < rowstart + 3)):
                                    continue
                                self.b[9 * i + c].candidates.discard(dig)
        return
    
    def solve(self, numIterations = 0):
        if (numIterations == 0):
            valid = False
            while not valid:
                self.searchForCandSingles()
                self.searchForCandPairs()
                self.searchForLockedCandsInBlocks()
                valid = self.valid()
        else:
            for i in range(numIterations):
                self.searchForCandSingles()
                self.searchForCandPairs()
                self.searchForLockedCandsInBlocks()
    


sb = SudokuBoard(testboard1)
sb.updateCands()

#cell[9 * 1 + 1]: hidden single
#cell[9 * 4 + 4]: naked single
#cell[9 * 6 + 8] and cell[9 * 8 + 8]: hidden pair (5, 7)
#cell[9 * 6 + 8] and cell[9 * 8 + 8]: naked pair (5, 7), check this with cell[9 * 0 + 8]

sb.solve(numIterations = 2)

print("Digit and Cands of cell(1, 1):")
print([sb.b[9 * 1 + 1].val, sb.b[9 * 1 + 1].candidates])
print("Digit and Cands of cell(4, 4):")
print([sb.b[9 * 4 + 4].val, sb.b[9 * 4 + 4].candidates])
print("Digit and Cands of cell(6, 8):")
print([sb.b[9 * 6 + 8].val, sb.b[9 * 6 + 8].candidates])
print("Digit and Cands of cell(8, 8):")
print([sb.b[9 * 8 + 8].val, sb.b[9 * 8 + 8].candidates])
print("Digit and Cands of cell(0, 8):")
print([sb.b[9 * 0 + 8].val, sb.b[9 * 0 + 8].candidates])

#sb.print()