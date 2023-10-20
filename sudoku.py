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
    [8, 4, 0,   6, 5, 0,   0, 2, 0],
    [0, 0, 3,   0, 0, 0,   0, 0, 0],
    [0, 2, 0,   0, 0, 7,   0, 0, 0],

    [0, 0, 0,   0, 1, 0,   0, 6, 0],
    [0, 0, 0,   0, 6, 0,   0, 0, 8],
    [9, 0, 6,   0, 0, 4,   0, 0, 1],

    [0, 5, 0,   3, 7, 0,   0, 8, 0],
    [0, 0, 0,   8, 0, 0,   3, 0, 7],
    [0, 0, 0,   0, 0, 9,   4, 0, 0]
    ]
testboard3 = [
    [0, 0, 5,   6, 3, 9,   0, 0, 0],
    [0, 7, 3,   0, 0, 0,   0, 0, 4],
    [0, 0, 0,   0, 0, 0,   0, 0, 0],

    [1, 0, 6,   9, 0, 5,   0, 0, 0],
    [0, 0, 0,   0, 0, 0,   0, 6, 0],
    [0, 9, 0,   0, 8, 0,   0, 7, 0],

    [8, 6, 9,   1, 0, 0,   2, 0, 3],
    [0, 1, 0,   0, 0, 2,   0, 0, 0],
    [0, 0, 0,   0, 0, 0,   8, 0, 0]
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
    
    def __str__(self) -> str:
        return "Cell(" + str(self.row) + ", " + str(self.col) + "): " + str([self.val, self.candidates])
    
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
        self.solvingSteps = list()
    
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

    def valid(self):
        for i in range(81):
            if self.b[i].isGap():
                return False
        exist = True
        for dig in range(1, 10):
            for i_ in range(9):
                exist = exist and self.isInBlock(i_, dig) and self.isInCol(i_, dig) and self.isInRow(i_, dig)
        return exist

    def print(self):
        for row in range(9):
            print("{}  {}  {}    {}  {}  {}    {}  {}  {}".format(*[self.at(row, c).val for c in range(9)]))
            if (row % 3 == 2):
                print("")

    def printSolvingSteps(self, printFullSteps = True):
        for k in range(len(self.solvingSteps)):
            assert(len(self.solvingSteps[k]) == 4)
            if printFullSteps:
                print("({}, {}): {}".format(self.solvingSteps[k][0],self.solvingSteps[k][1],self.solvingSteps[k][2]))
            else:
                if (self.solvingSteps[k][3]):
                    print("({}, {}): {}".format(self.solvingSteps[k][0],self.solvingSteps[k][1],self.solvingSteps[k][2]))
    
    def collectCands(self):
        for idx in range(81):
            if not self.b[idx].isGap():
                continue
            row = self.b[idx].row
            col = self.b[idx].col
            blk = self.b[idx].blk
            for dig in range(1, 10):
                if not(self.isInRow(row, dig) or self.isInCol(col, dig) or self.isInBlock(blk, dig)):
                    self.b[idx].candidates.add(dig)
    
    def updateCandsFromSolvedCell(self, row, col):
        newDigit = self.at(row, col).val
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
    
    def updateCandsInRow(self, row, excludedPositions: list, dig) -> bool:
        couldReduce = False
        for j in range(9):
            if (j in excludedPositions):
                continue
            if (dig in self.at(row, j).candidates):
                couldReduce = True
            self.at(row, j).candidates.discard(dig)
        return couldReduce
    
    def updateCandsInCol(self, col, excludedPositions: list, dig) -> bool:
        couldReduce = False
        for i in range(9):
            if (i in excludedPositions):
                continue
            if (dig in self.at(i, col).candidates):
                couldReduce = True
            self.at(i, col).candidates.discard(dig)
        return couldReduce
    
    def updateCandsInBlock(self, blk, excludedPositions: list, dig) -> bool:
        couldReduce = False
        for idx in range(9):
            if (idx in excludedPositions):
                continue
            if (dig in self.atBlock(blk, idx).candidates):
                couldReduce = True
            self.atBlock(blk, idx).candidates.discard(dig)
        return couldReduce

    def checkCellForNakedSingle(self, row, col) -> bool:
        if (self.at(row, col).lc() != 1):
            return False
        dig = self.at(row, col).candidates.pop()
        self.at(row, col).val = dig
        self.updateCandsFromSolvedCell(row, col)
        self.solvingSteps.append([row, col, "Naked Single", True])
        return True
    
    def checkCellForHiddenSingle(self, row, col) -> bool:
        if (self.at(row, col).lc() < 2):
            return False
        #search along row and collect all other candidates
        allOtherCands = set()
        for c in range(9):
            if (c == col or not self.at(row, c).isGap()):
                continue
            allOtherCands = allOtherCands.union(self.at(row, c).candidates)
        t = self.at(row, col).candidates.difference(allOtherCands)
        if (len(t) == 1):
            self.at(row, col).candidates.clear()
            self.at(row, col).val = t.pop()
            self.updateCandsFromSolvedCell(row, col)
            self.solvingSteps.append([row, col, "Hidden Single", True])
            return True
        #search along col and collect all other candidates
        allOtherCands = set()
        for r in range(9):
            if (r == row or not self.at(r, col).isGap()):
                continue
            allOtherCands = allOtherCands.union(self.at(r, col).candidates)
        t = self.at(row, col).candidates.difference(allOtherCands)
        if (len(t) == 1):
            self.at(row, col).candidates.clear()
            self.at(row, col).val = t.pop()
            self.updateCandsFromSolvedCell(row, col)
            self.solvingSteps.append([row, col, "Hidden Single", True])
            return True
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
            self.at(row, col).candidates.clear()
            self.at(row, col).val = t.pop()
            self.updateCandsFromSolvedCell(row, col)
            self.solvingSteps.append([row, col, "Hidden Single", True])
            return True
        return False
    
    def checkCellForNakedPair(self, row, col) -> bool:
        if (self.at(row, col).lc() != 2):
            return False
        candPair = self.b[9 * row + col].candidates.copy()
        rowpair = -1
        colpair = -1
        for idx in range(9):
            #if cell(row, col) can basically be a naked pair, look for the other part in same row
            c = idx
            if (c != col):
                if (self.at(row, c).candidates == candPair):
                    colpair = c
                stepReducedCands = False
                if (colpair >= 0):
                    while (len(candPair) > 0):
                        dig = candPair.pop()
                        stepReducedCands |= self.updateCandsInRow(row, [col, colpair], dig)
                    self.solvingSteps.append([row, col, "Naked Pair with cell" + str([row, colpair]), stepReducedCands])
                    return True
            #if cell(row, col) can basically be a naked pair, look for the other part in same col
            r = idx
            if (r != row):
                if (self.at(r, col).candidates == candPair):
                    rowpair = r
                stepReducedCands = False
                if (rowpair >= 0):
                    while (len(candPair) > 0):
                        dig = candPair.pop()
                        stepReducedCands |= self.updateCandsInCol(col, [row, rowpair], dig)
                    self.solvingSteps.append([row, col, "Naked Pair with cell" + str([rowpair, col]), stepReducedCands])
                    return True
            #if cell(row, col) can basically be a naked pair, look for the other part in same block
            blkIdx = idx
            blk = self.at(row, col).blk
            rowstart = self.at(row, col).rowBlkPos
            colstart = self.at(row, col).colBlkPos
            r = rowstart + (blkIdx // 3)
            c = colstart + (blkIdx % 3)
            if (r != row or c != col):
                if (self.at(r, c).candidates == candPair):
                    rowpair = r
                    colpair = c
                stepReducedCands = False
                if (rowpair >= 0 and colpair >= 0):
                    while (len(candPair) > 0):
                        dig = candPair.pop()
                        stepReducedCands |= self.updateCandsInBlock(blk, [self.at(row, col).blkidx, self.at(rowpair, colpair).blkidx], dig)
                    self.solvingSteps.append([row, col, "Naked Pair with cell" + str([rowpair, colpair]), stepReducedCands])
                    return True
        return False
    
    def checkCellForHiddenPair(self, row, col) -> bool:
        if (self.at(row, col).lc() < 3):
            return False
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
                    self.at(row, col).candidates = t.copy()
                    self.at(row, c).candidates = t.copy()
                    self.solvingSteps.append([row, col, "Hidden Pair " + str(t) + " with cell" + str([row, c]), True])
                    return True
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
                    self.at(row, col).candidates = t.copy()
                    self.at(r, col).candidates = t.copy()
                    self.solvingSteps.append([row, col, "Hidden Pair " + str(t) + " with cell" + str([r, col]), True])
                    return True
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
                    self.at(row, col).candidates = t.copy()
                    self.at(r, c).candidates = t.copy()
                    self.solvingSteps.append([row, col, "Hidden Pair " + str(t) + " with cell" + str([r, c]), True])
                    return True
        return False
    
    def checkCellForNakedTriplet(self, row, col) -> bool:
        if (self.at(row, col).lc() != 2 and self.at(row, col).lc() != 3):
            return False
        for idx0 in range(9):
            for idx1 in range(9):
                #if cell(row, col) can basically be a naked triple, look for the other part in same row
                c0 = idx0
                c1 = idx1
                if (c0 != col and c1 != col and c0 != c1 and (self.at(row, c0).lc() > 1) and (self.at(row, c1).lc() > 1)):
                    u = self.at(row, col).candidates
                    u = u.union(self.at(row, c0).candidates)
                    u = u.union(self.at(row, c1).candidates)
                    stepReducedCands = False
                    if (len(u) == 3):
                        while (len(u) > 0):
                            dig = u.pop()
                            stepReducedCands |= self.updateCandsInRow(row, [col, c0, c1], dig)
                        self.solvingSteps.append([row, col, "Naked Triplet with cell" + str([row, c0]) + " and cell" + str([row, c1]), stepReducedCands])
                        return True
                #if cell(row, col) can basically be a naked triple, look for the other part in same col
                r0 = idx0
                r1 = idx1
                if (r0 != row and r1 != row and r0 != r1 and (self.at(r0, col).lc() > 1) and (self.at(r1, col).lc() > 1)):
                    u = self.at(row, col).candidates
                    u = u.union(self.at(r0, col).candidates)
                    u = u.union(self.at(r1, col).candidates)
                    stepReducedCands = False
                    if (len(u) == 3):
                        while (len(u) > 0):
                            dig = u.pop()
                            stepReducedCands |= self.updateCandsInCol(col, [row, r0, r1], dig)
                        self.solvingSteps.append([row, col, "Naked Triplet with cell" + str([r0, col]) + " and cell" + str([r1, col]), stepReducedCands])
                        return True
        return False
    
    def checkCellForXWing(self, row, col) -> bool:
        if (not self.at(row, col).isGap()):
            return False
        for r in range(row + 1, 9, 1):
            for c in range(col + 1, 9, 1):
                if not(self.at(row, c).isGap() and self.at(r, col).isGap() and self.at(r, c).isGap()):
                    continue
                t = self.at(row, col).candidates.copy()
                t = t.intersection(self.at(row, c).candidates)
                t = t.intersection(self.at(r, col).candidates)
                t = t.intersection(self.at(r, c).candidates)
                #check if the x pattern has a common candidate
                if (len(t) == 0):
                    continue
                #check conditions for x-wing column-wise
                allOtherCands = set()
                for j in range(9):
                    if (j != col and j != c):
                        allOtherCands = allOtherCands.union(self.at(row, j).candidates)
                        allOtherCands = allOtherCands.union(self.at(r, j).candidates)
                tt = t.difference(allOtherCands)
                stepReducedCands = False
                if (len(tt) == 1):
                    dig = tt.pop()
                    stepReducedCands |= self.updateCandsInCol(col, [row, r], dig)
                    stepReducedCands |= self.updateCandsInCol(c, [row, r], dig)
                    self.solvingSteps.append([row, col, "X-Wing", stepReducedCands])
                    return True
                #check conditions for x-wing row-wise
                allOtherCands = set()
                for i in range(9):
                    if (i != row and i != r):
                        allOtherCands = allOtherCands.union(self.at(i, col).candidates)
                        allOtherCands = allOtherCands.union(self.at(i, c).candidates)
                tt = t.difference(allOtherCands)
                stepReducedCands = False
                if (len(tt) == 1):
                    dig = tt.pop()
                    stepReducedCands |= self.updateCandsInRow(row, [col, c], dig)
                    stepReducedCands |= self.updateCandsInRow(r, [col, c], dig)
                    self.solvingSteps.append([row, col, "X-Wing", stepReducedCands])
                    return True
        return False
    
    def checkCellForLockedCandsInBlocks(self, row, col) -> bool:
        if (self.at(row, col).lc() < 2):
            return False
        rowstart = self.at(row, col).rowBlkPos
        colstart = self.at(row, col).colBlkPos
        #check if current cand list has unique elements within all other block rows
        allOtherCands = set()
        for i in range(rowstart, rowstart + 3):
            for j in range(colstart, colstart + 3):
                if (i == row or not self.at(i, j).isGap()):
                    continue
                allOtherCands = allOtherCands.union(self.at(i, j).candidates)
        lockedCands = self.at(row, col).candidates.difference(allOtherCands)
        stepReducedCands = False
        if (len(lockedCands) > 0):
            while(len(lockedCands) > 0):
                dig = lockedCands.pop()
                stepReducedCands = self.updateCandsInRow(row, [colstart, colstart + 1, colstart + 2], dig)
                self.solvingSteps.append([row, col, "Cand " + str(dig) + " is locked in row", stepReducedCands])
            return True
        #check if current cand list has unique elements within all other block cols
        allOtherCands = set()
        for i in range(rowstart, rowstart + 3):
            for j in range(colstart, colstart + 3):
                if (j == col or not self.at(i, j).isGap()):
                    continue
                allOtherCands = allOtherCands.union(self.at(i, j).candidates)
        lockedCands = self.at(row, col).candidates.difference(allOtherCands)
        stepReducedCands = False
        if (len(lockedCands) > 0):
            while(len(lockedCands) > 0):
                dig = lockedCands.pop()
                stepReducedCands = self.updateCandsInCol(col, [rowstart, rowstart + 1, rowstart + 2], dig)
                self.solvingSteps.append([row, col, "Cand " + str(dig) + " is locked in col", stepReducedCands])
            return True
        return False
    
    def applyStrategies(self):
        for row in range(9):
            for col in range(9):
                if (self.at(row, col).isGap() == False):
                    assert(self.at(row, col).lc() == 0)
                    continue
                if self.checkCellForNakedSingle(row, col):
                    continue
                if self.checkCellForHiddenSingle(row, col):
                    continue
                if self.checkCellForNakedPair(row, col):
                    continue
                if self.checkCellForHiddenPair(row, col):
                    continue
                if self.checkCellForNakedTriplet(row, col):
                    continue
        for row in range(9):
            for col in range(9):
                if (self.at(row, col).isGap() == False):
                    assert(self.at(row, col).lc() == 0)
                    continue
                if self.checkCellForNakedSingle(row, col):
                    continue
                if self.checkCellForHiddenSingle(row, col):
                    continue
                if self.checkCellForLockedCandsInBlocks(row, col):
                    continue
                if self.checkCellForXWing(row, col):
                    continue
    
    def solve(self, numIterations = 0):
        if (numIterations == 0):
            valid = False
            while not valid:
                self.applyStrategies()
                valid = self.valid()
        else:
            for i in range(numIterations):
                self.applyStrategies()
    


sb = SudokuBoard(testboard3)
sb.collectCands()
#Do we need printings from cand lists?
testprintings = False
if (testprintings):
    ####
    sb.solve(numIterations = 2)
    ####
    print(sb.at(1, 1))
    print(sb.at(4, 4))
    print(sb.at(6, 8))
    print(sb.at(8, 8))
    print(sb.at(0, 8))
    print(sb.at(5, 5))
else:
    sb.solve()
    sb.print()
    sb.printSolvingSteps(printFullSteps = False)