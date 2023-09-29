#define boards to test
#cell[9 * 1 + 1]: hidden single
#cell[9 * 4 + 4]: naked single
#cell[9 * 6 + 8] and cell[9 * 8 + 8]: hidden pair (5, 7)
#cell[9 * 6 + 8] and cell[9 * 8 + 8]: naked pair (5, 7), check this with cell[9 * 0 + 8]
#cell[9 * 5 + 0], cell[9 * 5 + 1] and cell[9 * 5 + 2]: naked triplet (4, 8, 9), check this with cell[9 * 5 + 5]
testboard1 = [
    [0, 0, 0,   0, 4, 0,   2, 0, 0],
    [1, 0, 3,   0, 7, 0,   0, 0, 0],
    [0, 0, 0,   0, 2, 0,   0, 0, 0],

    [3, 6, 7,   0, 0, 0,   0, 0, 0],
    [5, 1, 2,   0, 0, 0,   0, 8, 9],
    [0, 0, 0,   0, 0, 0,   0, 0, 0],

    [0, 0, 0,   0, 0, 0,   4, 1, 0],
    [7, 0, 0,   2, 3, 5,   0, 0, 0],
    [0, 0, 0,   0, 0, 0,   3, 2, 0]
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
        self.row = idx // 9
        self.col = idx % 9
        self.blk = 3 * (self.row // 3) + (self.col // 3)
        self.blkidx = 3 * (self.row % 3) + (self.col % 3)
        #upper left cell in current subblock
        self.rowBlkPos = 3 * (self.row // 3)
        self.colBlkPos = 3 * (self.col // 3)
        #set for storing candidates
        self.candidates = set()
    
    def isEq(self, dig) -> bool:
        return (self.val == dig)
    
    def isGap(self) -> bool:
        return (self.val == 0)
    
    #length of cands set
    def lc(self) -> int:
        return len(self.candidates)


class SudokuBoard:
    def __init__(self, board):
        self.b = [Cell(i, board[i // 9][i % 9]) for i in range(81)]
    
    def at(self, row, col) -> Cell:
        assert(col >= 0 and col < 9)
        assert(row >= 0 and row < 9)
        return self.b[9 * row + col]
        
    def atBlock(self, block, index) -> Cell:
        assert(block >= 0 and block < 9)
        assert(index >= 0 and index < 9)
        row_ = 3 * (block // 3) + (index // 3)
        col_ = 3 * (block % 3) + (index % 3)
        return self.b[9 * row_ + col_]
        
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
            found = (self.atBlock(block, i).isEq(digit))
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
            print("{}  {}  {}    {}  {}  {}    {}  {}  {}".format(self.at(row, 0).val,self.at(row, 1).val,self.at(row, 2).val,self.at(row, 3).val,self.at(row, 4).val,self.at(row, 5).val,self.at(row, 6).val,self.at(row, 7).val,self.at(row, 8).val))
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
    
    def updateCandsFromSolvedCell(self, row, col, newDigit):
        #update cand list in same row
        for c in range(9):
            if (c == col):
                continue
            self.at(row, c).candidates.discard(newDigit)
        #update cand list in same col
        for r in range(9):
            if (r == row):
                continue
            self.at(r, col).candidates.discard(newDigit)
        #update cand list in same block
        rowstart = self.at(row, col).rowBlkPos
        colstart = self.at(row, col).colBlkPos
        for r in range(rowstart, rowstart + 3):
            for c in range(colstart, colstart + 3):
                if (r == row and c == col):
                    continue
                self.at(r, c).candidates.discard(newDigit)
        return
    
    def checkCellForNakedSingle(self, row, col):
        if (len(self.at(row, col).candidates) != 1):
            return
        dig = self.at(row, col).candidates.pop()
        self.at(row, col).val = dig
        self.updateCandsFromSolvedCell(row, col, dig)
        return
    
    def checkCellForHiddenSingle(self, row, col):
        if (not self.at(row, col).isGap()):
            return
        #search along row and collect all other candidates
        allOtherCands = set()
        for c in range(9):
            if (c == col or not self.at(row, c).isGap()):
                continue
            allOtherCands = allOtherCands.union(self.at(row, c).candidates)
        t = self.at(row, col).candidates.difference(allOtherCands)
        if (len(t) == 1):
            self.at(row, col).candidates = t
            dig = self.at(row, col).candidates.pop()
            self.at(row, col).val = dig
            self.updateCandsFromSolvedCell(row, col, dig)
            return
        #search along col and collect all other candidates
        allOtherCands = set()
        for r in range(9):
            if (r == row or not self.at(r, col).isGap()):
                continue
            allOtherCands = allOtherCands.union(self.at(r, col).candidates)
        t = self.at(row, col).candidates.difference(allOtherCands)
        if (len(t) == 1):
            self.at(row, col).candidates = t
            dig = self.at(row, col).candidates.pop()
            self.at(row, col).val = dig
            self.updateCandsFromSolvedCell(row, col, dig)
            return
        #search within block and collect all other candidates
        allOtherCands = set()
        rowstart = self.at(row, col).rowBlkPos
        colstart = self.at(row, col).colBlkPos
        for r in range(rowstart, rowstart + 3):
            for c in range(colstart, colstart + 3):
                if ((r == row and c == col) or not self.at(r, c).isGap()):
                    continue
                allOtherCands = allOtherCands.union(self.at(r, c).candidates)
        t = self.at(row, col).candidates.difference(allOtherCands)
        if (len(t) == 1):
            self.at(row, col).candidates = t
            dig = self.at(row, col).candidates.pop()
            self.at(row, col).val = dig
            self.updateCandsFromSolvedCell(row, col, dig)
            return
    
    def checkCellForNakedPair(self, row, col):
        if (self.at(row, col).lc() != 2):
            return
        candPair = self.b[9 * row + col].candidates
        rowpair = -1
        colpair = -1
        for idx in range(9):
            #if cell(row, col) can basically be a naked pair, look for the other part in same row
            c = idx
            if (c != col):
                if (self.at(row, c).candidates == candPair):
                    colpair = c
                if (colpair >= 0):
                    for j in range(9):
                        if ((j == col) or (j == colpair)):
                            continue
                        self.at(row, j).candidates.difference_update(candPair)
                    return
            #if cell(row, col) can basically be a naked pair, look for the other part in same col
            r = idx
            if (r != row):
                if (self.at(r, col).candidates == candPair):
                    rowpair = r
                if (rowpair >= 0):
                    for i in range(9):
                        if ((i == row) or (i == rowpair)):
                            continue
                        self.at(i, col).candidates.difference_update(candPair)
                    return
            #if cell(row, col) can basically be a naked pair, look for the other part in same block
            blkIdx = idx
            rowstart = self.at(row, col).rowBlkPos
            colstart = self.at(row, col).colBlkPos
            r = rowstart + (blkIdx // 3)
            c = colstart + (blkIdx % 3)
            if (r != row or c != col):
                if (self.at(r, c).candidates == candPair):
                    rowpair = r
                    colpair = c
                if (rowpair >= 0 and colpair >= 0):
                    for i in range(rowstart, rowstart + 3):
                        for j in range(colstart, colstart + 3):
                            if ((i == row and j == col) or (i == rowpair and j == colpair)):
                                continue
                            self.at(i, j).candidates.difference_update(candPair)
                    return
    
    def checkCellForHiddenPair(self, row, col):
        if (not self.at(row, col).isGap()):
            return
        for idx in range(9):
            #search for a pair along row
            c = idx
            if (c != col and self.at(row, c).isGap()):
                #collect all other candidates
                allOtherCands = set()
                for j in range(9):
                    if (j == col or j == c):
                        continue
                    allOtherCands = allOtherCands.union(self.at(row, j).candidates)
                t = self.at(row, col).candidates.intersection(self.at(row, c).candidates)
                t = t.difference(allOtherCands)
                if (len(t) == 2):
                    self.at(row, col).candidates = t
                    self.at(row, c).candidates = t
                    return
            #search for a pair along col
            r = idx
            if (r != row and self.at(r, col).isGap()):
                #collect all other candidates
                allOtherCands = set()
                for i in range(9):
                    if (i == row or i == r):
                        continue
                    allOtherCands = allOtherCands.union(self.at(i, col).candidates)
                t = self.at(row, col).candidates.intersection(self.at(r, col).candidates)
                t = t.difference(allOtherCands)
                if (len(t) == 2):
                    self.at(row, col).candidates = t
                    self.at(r, col).candidates = t
                    return
            #search the pair within block
            blkIdx = idx
            rowstart = self.at(row, col).rowBlkPos
            colstart = self.at(row, col).colBlkPos
            r = rowstart + (blkIdx // 3)
            c = colstart + (blkIdx % 3)
            if ((r != row or c != col) and self.at(r, c).isGap()):
                #collect all other candidates
                allOtherCands = set()
                for i in range(rowstart, rowstart + 3):
                    for j in range(colstart, colstart + 3):
                        if ((i == row and j == col) or (i == r and j == c)):
                            continue
                        allOtherCands = allOtherCands.union(self.at(i, j).candidates)
                t = self.at(row, col).candidates.intersection(self.at(r, c).candidates)
                t = t.difference(allOtherCands)
                if (len(t) == 2):
                    self.at(row, col).candidates = t
                    self.at(r, c).candidates = t
                    return
    
    def checkCellForNakedTriplet(self, row, col):
        if (self.at(row, col).lc() != 2 and self.at(row, col).lc() != 3):
            return
        rowpair = [-1, -1]
        colpair = [-1, -1]
        for idx0 in range(9):
            for idx1 in range(9):
                #if cell(row, col) can basically be a naked triple, look for the other part in same row
                c0 = idx0
                c1 = idx1
                if (c0 != col and c1 != col and c0 != c1 and (self.at(row, c0).lc() > 1) and (self.at(row, c1).lc() > 1)):
                    u = self.at(row, col).candidates
                    u = u.union(self.at(row, c0).candidates)
                    u = u.union(self.at(row, c1).candidates)
                    if (len(u) == 3):
                        colpair = [c0, c1]
                    if (colpair != [-1, -1]):
                        for j in range(9):
                            if ((j == col) or (j == colpair[0]) or (j == colpair[1])):
                                continue
                            self.at(row, j).candidates.difference_update(u)
                        return
                #if cell(row, col) can basically be a naked triple, look for the other part in same col
                r0 = idx0
                r1 = idx1
                if (r0 != row and r1 != row and r0 != r1 and (self.at(r0, col).lc() > 1) and (self.at(r1, col).lc() > 1)):
                    u = self.at(row, col).candidates
                    u = u.union(self.at(r0, col).candidates)
                    u = u.union(self.at(r1, col).candidates)
                    if (len(u) == 3):
                        rowpair = [r0, r1]
                    if (rowpair != [-1, -1]):
                        for i in range(9):
                            if ((i == row) or (i == rowpair[0]) or (i == rowpair[1])):
                                continue
                            self.at(i, col).candidates.difference_update(u)
                        return
    
    def searchForNakedTuples(self):
        for row in range(9):
            for col in range(9):
                self.checkCellForNakedSingle(row, col)
                self.checkCellForNakedPair(row, col)
                self.checkCellForNakedTriplet(row, col)
        return

    def searchForHiddenTuples(self):
        for row in range(9):
            for col in range(9):
                self.checkCellForHiddenSingle(row, col)
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
                        if self.at(r, c).isGap():
                            numGaps = numGaps + 1
                            lockedCands = lockedCands.union(self.at(r, c).candidates)
                    if (numGaps > 1):
                        #check if lockedCands is not part of other cand lists
                        allOtherCands = set()
                        for i in range(rowstart, rowstart + 3):
                            for j in range(colstart, colstart + 3):
                                if (i == r or not self.at(i, j).isGap()):
                                    continue
                                allOtherCands = allOtherCands.union(self.at(i, j).candidates)
                        t = lockedCands.difference(allOtherCands)
                        if (len(t) == 1):
                            dig = t.pop()
                            for j in range(9):
                                if ((j >= colstart) and (j < colstart + 3)):
                                    continue
                                self.at(r, j).candidates.discard(dig)
                #go through all cols in block
                for c in range(colstart, colstart + 3):
                    numGaps = 0
                    lockedCands = set()
                    for r in range(rowstart, rowstart + 3):
                        if self.at(r, c).isGap():
                            numGaps = numGaps + 1
                            lockedCands = lockedCands.union(self.at(r, c).candidates)
                    if (numGaps > 1):
                        #check if lockedCands is not part of other cand lists
                        allOtherCands = set()
                        for i in range(rowstart, rowstart + 3):
                            for j in range(colstart, colstart + 3):
                                if (j == c or not self.at(i, j).isGap()):
                                    continue
                                allOtherCands = allOtherCands.union(self.at(i, j).candidates)
                        t = lockedCands.difference(allOtherCands)
                        if (len(t) == 1):
                            dig = t.pop()
                            for i in range(9):
                                if ((i >= rowstart) and (i < rowstart + 3)):
                                    continue
                                self.at(i, c).candidates.discard(dig)
        return
    
    def solve(self, numIterations = 0):
        if (numIterations == 0):
            valid = False
            while not valid:
                self.searchForNakedTuples()
                self.searchForHiddenTuples()
                self.searchForLockedCandsInBlocks()
                valid = self.valid()
        else:
            for i in range(numIterations):
                self.searchForNakedTuples()
                self.searchForHiddenTuples()
                self.searchForLockedCandsInBlocks()
    


sb = SudokuBoard(testboard1)
sb.updateCands()

#cell[9 * 1 + 1]: hidden single
#cell[9 * 4 + 4]: naked single
#cell[9 * 6 + 8] and cell[9 * 8 + 8]: hidden pair (5, 7)
#cell[9 * 6 + 8] and cell[9 * 8 + 8]: naked pair (5, 7), check this with cell[9 * 0 + 8]
#cell[9 * 5 + 0], cell[9 * 5 + 1] and cell[9 * 5 + 2]: naked triplet (4, 8, 9), check this with cell[9 * 5 + 5]

sb.solve(numIterations = 2)

print("Digit and Cands of cell(1, 1):")
print([sb.at(1, 1).val, sb.at(1, 1).candidates])
print("Digit and Cands of cell(4, 4):")
print([sb.at(4, 4).val, sb.at(4, 4).candidates])
print("Digit and Cands of cell(6, 8):")
print([sb.at(6, 8).val, sb.at(6, 8).candidates])
print("Digit and Cands of cell(8, 8):")
print([sb.at(8, 8).val, sb.at(8, 8).candidates])
print("Digit and Cands of cell(0, 8):")
print([sb.at(0, 8).val, sb.at(0, 8).candidates])
print("Digit and Cands of cell(5, 5):")
print([sb.at(5, 5).val, sb.at(5, 5).candidates])

#sb.print()