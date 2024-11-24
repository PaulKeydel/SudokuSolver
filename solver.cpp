#include "solver.h"
#include <cassert>
#include <iostream>

using namespace std;

Cell::Cell(int idx, int digit):
    val(digit),
    row(idx / 9),
    col(idx % 9),
    blk(3 * (row / 3) + (col / 3)),
    blkidx(3 * (row % 3) + (col % 3)),
    rowBlkPos(3 * (row / 3)),
    colBlkPos(3 * (col / 3)),
    pairColor(-1)
{};

string Cell::getOutput()
{
    return string("Cell(") + to_string(this->row) + string(", ") + to_string(this->col) + string("): ") + to_string(this->val);
}


SudokuBoard::SudokuBoard(int* board)
{
    for (int i = 0; i < 81; i++)
    {
        b.at(i) = Cell(i, board[i]); //move assignment
    }
}

set<int> operator-(set<int>& op1, set<int>& op2)
{
    set<int> dummy;
    set_difference(op1.begin(), op1.end(), op2.begin(), op2.end(), inserter(dummy, dummy.begin()));
    return dummy;
}

set<int> operator&&(set<int>& op1, set<int>& op2)
{
    set<int> dummy;
    set_intersection(op1.begin(), op1.end(), op2.begin(), op2.end(), inserter(dummy, dummy.begin()));
    return dummy;
}

set<int> operator||(set<int>& op1, set<int>& op2)
{
    set<int> result;
    result.insert(op1.begin(), op1.end());
    result.insert(op2.begin(), op2.end());
    return result;
}

void SudokuBoard::appendSolvStep(int row, int col, string text, bool bReducedCands)
{
    if (bReducedCands)
    {
        pair<int, string> p = make_pair(9 * row + col, text);
        solvingSteps.push_back(p);
    }
}

string SudokuBoard::cand2str(set<int> cands)
{
    string res = "{";
    for (int k = 0; k < cands.size(); k++)
    {
        auto ci = cands.begin();
        advance(ci, k);
        res += to_string(*ci);
        res += ((k == cands.size() - 1) ? "}" : ", ");
    }
    return res;
}

string SudokuBoard::cord2str(int row, int col)
{
    return "(" + to_string(row) + ", " + to_string(col) + ")";
}

void SudokuBoard::print()
{
    for (int row = 0; row < 9; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            cout << this->at(row, col).val << "  ";
            if (col % 3 == 2) cout << "  ";
        }
        cout << endl;
        if (row % 3 == 2) cout << endl;
    }
}

void SudokuBoard::printSolvingSteps()
{
    for (int k = 0; k < solvingSteps.size(); k++)
    {
        int row = get<0>(solvingSteps.at(k)) / 9;
        int col = get<0>(solvingSteps.at(k)) % 9;
        string text = get<1>(solvingSteps.at(k));
        cout << "(" << to_string(row) << ", " << to_string(col) << "): " << text << endl;
    }    
}

Cell& SudokuBoard::at(int row, int col)
{
    assert(col >= 0 && col < 9);
    assert(row >= 0 && row < 9);
    return b.at(9 * row + col);
}

Cell& SudokuBoard::atBlock(int block, int index)
{
    assert(block >= 0 && block < 9);
    assert(index >= 0 && index < 9);
    int row_ = 3 * (block / 3) + (index / 3);
    int col_ = 3 * (block % 3) + (index % 3);
    return b.at(9 * row_ + col_);
}

bool SudokuBoard::isInCol(int col, int digit)
{
    assert(col >= 0 && col < 9);
    int i = 0;
    bool found = false;
    while (i < 9 && !found)
    {
        found = b.at(9 * i + col).isEq(digit);
        i = i + 1;
    }
    return found;
}

bool SudokuBoard::isInRow(int row, int digit)
{
    assert(row >= 0 && row < 9);
    int i = 0;
    bool found = false;
    while (i < 9 && !found)
    {
        found = b.at(9 * row + i).isEq(digit);
        i = i + 1;
    }
    return found;
}

bool SudokuBoard::isInBlock(int block, int digit)
{
    assert(block >= 0 && block < 9);
    int i = 0;
    bool found = false;
    while (i < 9 && !found)
    {
        found = (atBlock(block, i).isEq(digit));
        i = i + 1;
    }
    return found;
}

bool SudokuBoard::valid()
{
    for (int i = 0; i < 81; i++)
    {
        if (b.at(i).isGap()) return false;
    }
    bool exist = true;
    for (int dig = 1; dig < 10; dig++)
    {
        for (int i_ = 0; i_ < 9; i_++)
        {
            exist = exist && isInBlock(i_, dig) && isInCol(i_, dig) && isInRow(i_, dig);
        }
    }
    return exist;
}

void SudokuBoard::collectCands()
{
    for (int idx = 0; idx < 81; idx++)
    {
        if (!b.at(idx).isGap()) continue;
        int row = b.at(idx).row;
        int col = b.at(idx).col;
        int blk = b.at(idx).blk;
            for (int dig = 1; dig < 10; dig++)
            {
                if (!(isInRow(row, dig) || isInCol(col, dig) || isInBlock(blk, dig)))
                {
                    b.at(idx).candidates.insert(dig);
                }
            }
    }
}

bool SudokuBoard::updateCandsInRow(int row, vector<int> excludedPositions, int dig)
{
    bool couldReduce = false;
    for (int j = 0; j < 9; j++)
    {
        if (find(excludedPositions.begin(), excludedPositions.end(), j) != excludedPositions.end())
        {
            continue;
        }
        if (at(row, j).hasC(dig))
        {
            couldReduce |= true;
            at(row, j).candidates.erase(dig);
        }
    }
    return couldReduce;
}

bool SudokuBoard::updateCandsInCol(int col, vector<int> excludedPositions, int dig)
{
    bool couldReduce = false;
    for (int i = 0; i < 9; i++)
    {
        if (find(excludedPositions.begin(), excludedPositions.end(), i) != excludedPositions.end())
        {
            continue;
        }
        if (at(i, col).hasC(dig))
        {
            couldReduce |= true;
            at(i, col).candidates.erase(dig);
        }
    }
    return couldReduce;
}

bool SudokuBoard::updateCandsInBlock(int blk, vector<int> excludedPositions, int dig)
{
    bool couldReduce = false;
    for (int idx = 0; idx < 9; idx++)
    {
        if (find(excludedPositions.begin(), excludedPositions.end(), idx) != excludedPositions.end())
        {
            continue;
        }
        if (atBlock(blk, idx).hasC(dig))
        {
            couldReduce |= true;
            atBlock(blk, idx).candidates.erase(dig);
        }
    }
    return couldReduce;
}

void SudokuBoard::updateCandsFromSolvedCell(int row, int col)
{
    int newDigit = at(row, col).val;
    assert(newDigit > 0 && newDigit < 10);
    //update cand list in same row
    updateCandsInRow(row, vector<int>{col}, newDigit);
    //update cand list in same col
    updateCandsInCol(col, vector<int>{row}, newDigit);
    //update cand list in same block
    updateCandsInBlock(at(row, col).blk, vector<int>{at(row, col).blkidx}, newDigit);
}

bool SudokuBoard::checkCellForNakedSingle(int row, int col)
{
    if (at(row, col).lc() != 1)
    {
        return false;
    }
    int dig = *at(row, col).candidates.begin();
    at(row, col).candidates.erase(dig);
    at(row, col).val = dig;
    updateCandsFromSolvedCell(row, col);
    appendSolvStep(row, col, "Naked Single", true);
    return true;
}

bool SudokuBoard::checkCellForHiddenSingle(int row, int col)
{
    if (at(row, col).lc() < 2)
    {
        return false;
    }
    //search along row and collect all other candidates
    set<int> allOtherCands, t;
    for (int c = 0; c < 9; c++)
    {
        if (c == col || !at(row, c).isGap()) continue;
        allOtherCands = allOtherCands || at(row, c).candidates;
    }
    t = at(row, col).candidates - allOtherCands;
    if (t.size() == 1)
    {
        at(row, col).candidates.clear();
        at(row, col).val = *t.begin();
        updateCandsFromSolvedCell(row, col);
        appendSolvStep(row, col, "Hidden Single", true);
        return true;
    }
    //search along col and collect all other candidates
    allOtherCands.clear();
    for (int r = 0; r < 9; r++)
    {
        if (r == row || !at(r, col).isGap()) continue;
        allOtherCands = allOtherCands || at(r, col).candidates;
    }
    t = at(row, col).candidates - allOtherCands;
    if (t.size() == 1)
    {
        at(row, col).candidates.clear();
        at(row, col).val = *t.begin();
        updateCandsFromSolvedCell(row, col);
        appendSolvStep(row, col, "Hidden Single", true);
        return true;
    }
    //search within block and collect all other candidates
    allOtherCands.clear();
    int rowstart = at(row, col).rowBlkPos;
    int colstart = at(row, col).colBlkPos;
    for (int r = rowstart; r < rowstart + 3; r++)
    {
        for (int c = colstart; c < colstart + 3; c++)
        {
            if ((r == row && c == col) || !at(r, c).isGap()) continue;
            allOtherCands = allOtherCands || at(r, c).candidates;
        }
    }
    t = at(row, col).candidates - allOtherCands;
    if (t.size() == 1)
    {
        at(row, col).candidates.clear();
        at(row, col).val = *t.begin();
        updateCandsFromSolvedCell(row, col);
        appendSolvStep(row, col, "Hidden Single", true);
        return true;
    }
    return false;
}

bool SudokuBoard::checkCellForNakedPair(int row, int col)
{
    if (at(row, col).lc() != 2)
    {
        return false;
    }
    set<int> candPair = b[9 * row + col].candidates;
    for (int idx = 0; idx < 9; idx++)
    {
        //if cell(row, col) can basically be a naked pair, look for the other part in same row
        int c = idx;
        bool stepReducedCands = false;
        if (c != col && at(row, c).candidates == candPair)
        {
            for (auto it = candPair.begin(); it != candPair.end(); ++it)
            {
                stepReducedCands |= updateCandsInRow(row, vector<int>{col, c}, *it);
            }
            appendSolvStep(row, col, "Naked Pair with cell (" + to_string(row) + ", " + to_string(c) + ")", stepReducedCands);
            return true;
        }
        //if cell(row, col) can basically be a naked pair, look for the other part in same col
        int r = idx;
        stepReducedCands = false;
        if (r != row && at(r, col).candidates == candPair)
        {
            for (auto it = candPair.begin(); it != candPair.end(); ++it)
            {
                stepReducedCands |= updateCandsInCol(col, vector<int>{row, r}, *it);
            }
            appendSolvStep(row, col, "Naked Pair with cell (" + to_string(r) + ", " + to_string(col) + ")", stepReducedCands);
            return true;
        }
        //if cell(row, col) can basically be a naked pair, look for the other part in same block
        int blkIdx = idx;
        int blk = at(row, col).blk;
        int rowstart = at(row, col).rowBlkPos;
        int colstart = at(row, col).colBlkPos;
        r = rowstart + (blkIdx / 3);
        c = colstart + (blkIdx % 3);
        stepReducedCands = false;
        if ((r != row || c != col) && at(r, c).candidates == candPair)
        {
            for (auto it = candPair.begin(); it != candPair.end(); ++it)
            {
                stepReducedCands |= updateCandsInBlock(blk, vector<int>{at(row, col).blkidx, at(r, c).blkidx}, *it);
            }
            appendSolvStep(row, col, "Naked Pair with cell (" + to_string(r) + ", " + to_string(c) + ")", stepReducedCands);
            return true;
        }
    }
    return false;
}

bool SudokuBoard::checkCellForHiddenPair(int row, int col)
{
    if (at(row, col).lc() < 3)
    {
        return false;
    }
    set<int> allOtherCands, t;
    for (int idx = 0; idx < 9; idx++)
    {
        //search for a pair along row
        int c = idx;
        if (c != col && at(row, c).isGap())
        {
            //collect all other candidates
            allOtherCands.clear();
            for (int j = 0; j < 9; j++)
            {
                if (j == col || j == c) continue;
                allOtherCands = allOtherCands || at(row, j).candidates;
            }
            t = at(row, col).candidates && at(row, c).candidates;
            t = t - allOtherCands;
            if (t.size() == 2)
            {
                at(row, col).candidates = t;
                at(row, c).candidates = t;
                appendSolvStep(row, col, "Hidden Pair " + cand2str(t) + " with cell " + cord2str(row, c), true);
                return true;
            }
        }
        //search for a pair along col
        int r = idx;
        if (r != row && at(r, col).isGap())
        {
            //collect all other candidates
            allOtherCands.clear();
            for (int i = 0; i < 9; i++)
            {
                if (i == row || i == r) continue;
                allOtherCands = allOtherCands || at(i, col).candidates;
            }
            t = at(row, col).candidates && at(r, col).candidates;
            t = t - allOtherCands;
            if (t.size() == 2)
            {
                at(row, col).candidates = t;
                at(r, col).candidates = t;
                appendSolvStep(row, col, "Hidden Pair " + cand2str(t) + " with cell " + cord2str(r, col), true);
                return true;
            }
        }
        //search the pair within block
        int blkIdx = idx;
        int rowstart = at(row, col).rowBlkPos;
        int colstart = at(row, col).colBlkPos;
        r = rowstart + (blkIdx / 3);
        c = colstart + (blkIdx % 3);
        if ((r != row || c != col) && at(r, c).isGap())
        {
            //collect all other candidates
            allOtherCands.clear();
            for (int i = rowstart; i < rowstart + 3; i++)
            {
                for (int j = colstart; j < colstart + 3; j++)
                {
                    if ((i == row && j == col) || (i == r && j == c)) continue;
                    allOtherCands = allOtherCands || at(i, j).candidates;
                }
            }
            t = at(row, col).candidates && at(r, c).candidates;
            t = t - allOtherCands;
            if (t.size() == 2)
            {
                at(row, col).candidates = t;
                at(r, c).candidates = t;
                appendSolvStep(row, col, "Hidden Pair " + cand2str(t) + " with cell " + cord2str(r, c), true);
                return true;
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForNakedTriplet(int row, int col)
{
    if (at(row, col).lc() != 2 && at(row, col).lc() != 3)
    {
        return false;
    }
    for (int idx0 = 0; idx0 < 9; idx0++)
    {
        for (int idx1 = 0; idx1 < 9; idx1++)
        {
            //if cell(row, col) can basically be a naked triple, look for the other part in same row
            int c0 = idx0;
            int c1 = idx1;
            if (c0 != col && c1 != col && c0 != c1 && (at(row, c0).lc() > 1) && (at(row, c1).lc() > 1))
            {
                set<int> u = at(row, col).candidates;
                u = u || at(row, c0).candidates;
                u = u || at(row, c1).candidates;
                bool stepReducedCands = false;
                if (u.size() == 3)
                {
                    for (auto it = u.begin(); it != u.end(); ++it)
                    {
                        stepReducedCands |= updateCandsInRow(row, vector<int>{col, c0, c1}, *it);
                    }
                    appendSolvStep(row, col, "Naked Triplet with cell " + cord2str(row, c0) + " and cell " + cord2str(row, c1), stepReducedCands);
                    return true;
                }
            }
            //if cell(row, col) can basically be a naked triple, look for the other part in same col
            int r0 = idx0;
            int r1 = idx1;
            if (r0 != row && r1 != row && r0 != r1 && (at(r0, col).lc() > 1) && (at(r1, col).lc() > 1))
            {
                set<int> u = at(row, col).candidates;
                u = u || at(r0, col).candidates;
                u = u || at(r1, col).candidates;
                bool stepReducedCands = false;
                if (u.size() == 3)
                {
                    for (auto it = u.begin(); it != u.end(); ++it)
                    {
                        stepReducedCands |= updateCandsInCol(col, vector<int>{row, r0, r1}, *it);
                    }
                    appendSolvStep(row, col, "Naked Triplet with cell " + cord2str(r0, col) + " and cell " + cord2str(r1, col), stepReducedCands);
                    return true;
                }
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForXWing(int row, int col)
{
    if (!at(row, col).isGap())
    {
        return false;
    }
    set<int> allOtherCands, t, tt;
    for (int r = row + 1; r < 9; r++)
    {
        for (int c = col + 1; c < 9; c++)
        {
            if (!(at(row, c).isGap() && at(r, col).isGap() && at(r, c).isGap())) continue;
            t = at(row, col).candidates;
            t = t && at(row, c).candidates;
            t = t && at(r, col).candidates;
            t = t && at(r, c).candidates;
            //check if the x pattern has a common candidate
            if (t.size() == 0) continue;
            //check conditions for x-wing column-wise
            allOtherCands.clear();
            for (int j = 0; j < 9; j++)
            {
                if (j != col && j != c)
                {
                    allOtherCands = allOtherCands || at(row, j).candidates;
                    allOtherCands = allOtherCands || at(r, j).candidates;
                }
            }
            tt = t - allOtherCands;
            bool stepReducedCands = false;
            if (tt.size() == 1)
            {
                stepReducedCands |= updateCandsInCol(col, vector<int>{row, r}, *tt.begin());
                stepReducedCands |= updateCandsInCol(c, vector<int>{row, r}, *tt.begin());
                appendSolvStep(row, col, "column-wise X-Wing with diag cell" + cord2str(r, c), stepReducedCands);
                return true;
            }
            //check conditions for x-wing row-wise
            allOtherCands.clear();
            for (int i = 0; i < 9; i++)
            {
                if (i != row && i != r)
                {
                    allOtherCands = allOtherCands || at(i, col).candidates;
                    allOtherCands = allOtherCands || at(i, c).candidates;
                }
            }
            tt = t - allOtherCands;
            stepReducedCands = false;
            if (tt.size() == 1)
            {
                stepReducedCands |= updateCandsInRow(row, vector<int>{col, c}, *tt.begin());
                stepReducedCands |= updateCandsInRow(r, vector<int>{col, c}, *tt.begin());
                appendSolvStep(row, col, "row-wise X-Wing with diag cell" + cord2str(r, c), stepReducedCands);
                return true;
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForXYWing(int row, int col)
{
    if (at(row, col).lc() != 2)
    {
        return false;
    }
    int blk = at(row, col).blk;
    int rowstart = at(row, col).rowBlkPos;
    int colstart = at(row, col).colBlkPos;
    set<int> t0, t1, t2;
    //check if we have a xy pattern in row-column scenario
    for (int r = row + 1; r < 9; r++)
    {
        for (int c = col + 1; c < 9; c++)
        {
            if (!(at(row, c).lc() == 2 && at(r, col).lc() == 2)) continue;
            t0 = at(row, col).candidates && at(row, c).candidates;
            t1 = at(row, col).candidates && at(r, col).candidates;
            t2 = at(row, c).candidates && at(r, col).candidates;
            if (t0.size() == 1 && t1.size() == 1 && t2.size() == 1 && t0 != t1 && t2 != t0 && t2 != t1)
            {
                int lcbefore = at(r, c).lc();
                at(r, c).candidates.erase(*t2.begin());
                int lcafter = at(r, c).lc();
                appendSolvStep(row, col, "XY wing", lcbefore > lcafter);
                return true;
            }
        }
    }
    //check if we have a xy pattern in block-column scenario
    for (int bi = 0; bi < 9; bi++)
    {
        if (at(blk, bi).row == row) continue;
        for (int c = 0; c < 9; c++)
        {
            if ((c >= colstart) && (c < colstart + 3)) continue;
            if (!(at(row, c).lc() == 2 && atBlock(blk, bi).lc() == 2)) continue;
            t0 = at(row, col).candidates && at(row, c).candidates;
            t1 = at(row, col).candidates && atBlock(blk, bi).candidates;
            t2 = at(row, c).candidates && atBlock(blk, bi).candidates;
            if (t0.size() == 1 && t1.size() == 1 && t2.size() == 1 && t0 != t1 && t2 != t0 && t2 != t1)
            {
                int r1 = atBlock(blk, bi).row;
                for (int c1 = at(row, c).colBlkPos; c1 < at(row, c).colBlkPos + 3; c1++)
                {
                    int lcbefore = at(r1, c1).lc();
                    at(r1, c1).candidates.erase(*t2.begin());
                    int lcafter = at(r1, c1).lc();
                    appendSolvStep(row, col, "XY wing", lcbefore > lcafter);
                }
            }
        }
    }
    //check if we have a xy pattern in block-row scenario
    for (int bi = 0; bi < 9; bi++)
    {
        if (at(blk, bi).col == col) continue;
        for (int r = 0; r < 9; r++)
        {
            if ((r >= rowstart) && (r < rowstart + 3)) continue;
            if (!(at(r, col).lc() == 2 && atBlock(blk, bi).lc() == 2)) continue;
            t0 = at(row, col).candidates && at(r, col).candidates;
            t1 = at(row, col).candidates && atBlock(blk, bi).candidates;
            t2 = at(r, col).candidates && atBlock(blk, bi).candidates;
            if (t0.size() == 1 && t1.size() == 1 && t2.size() == 1 && t0 != t1 && t2 != t0 && t2 != t1)
            {
                int c1 = atBlock(blk, bi).col;
                for (int r1 = at(r, col).rowBlkPos; r1 < at(r, col).rowBlkPos + 3; r1++)
                {
                    int lcbefore = at(r1, c1).lc();
                    at(r1, c1).candidates.erase(*t2.begin());
                    int lcafter = at(r1, c1).lc();
                    appendSolvStep(row, col, "XY wing", lcbefore > lcafter);
                }
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForLockedCandsInBlocks(int row, int col)
{
    if (at(row, col).lc() < 2)
    {
        return false;
    }
    int rowstart = at(row, col).rowBlkPos;
    int colstart = at(row, col).colBlkPos;
    //check if current cand list has unique elements within all other block rows
    set<int> allOtherCands, lockedCands;
    for (int i = rowstart; i < rowstart + 3; i++)
    {
        for (int j = colstart; j < colstart + 3; j++)
        {
            if (i == row || !at(i, j).isGap()) continue;
            allOtherCands = allOtherCands || at(i, j).candidates;
        }
    }
    lockedCands = at(row, col).candidates - allOtherCands;
    bool stepReducedCands = false;
    if (lockedCands.size() > 0)
    {
        for (auto it = lockedCands.begin(); it != lockedCands.end(); ++it)
        {
            stepReducedCands = updateCandsInRow(row, vector<int>{colstart, colstart + 1, colstart + 2}, *it);
            appendSolvStep(row, col, "Cand " + to_string(*it) + " is locked in row", stepReducedCands);
        }
        return true;
    }
    //check if current cand list has unique elements within all other block cols
    allOtherCands.clear();
    for (int i = rowstart; i < rowstart + 3; i++)
    {
        for (int j = colstart; j < colstart + 3; j++)
        {
            if (j == col || !at(i, j).isGap()) continue;
            allOtherCands = allOtherCands || at(i, j).candidates;
        }
    }
    lockedCands = at(row, col).candidates - allOtherCands;
    stepReducedCands = false;
    if (lockedCands.size() > 0)
    {
        for (auto it = lockedCands.begin(); it != lockedCands.end(); ++it)
        {
            stepReducedCands = updateCandsInCol(col, vector<int>{rowstart, rowstart + 1, rowstart + 2}, *it);
            appendSolvStep(row, col, "Cand " + to_string(*it) + " is locked in col", stepReducedCands);
        }
        return true;
    }
    return false;
}

void SudokuBoard::checkForIntersectingColorPairs(int row, int col, int row1, int col1, int color)
{
    if (at(row, col).lc() != 2)
    {
        return;
    }
    assert(color == 0 || color == 1);
    set<int> candPair = at(row, col).candidates;
    //reset field
    if (row1 == -1 && col1 == -1)
    {
        for (int r = 0; r < 9; r++)
        {
            for (int c = 0; c < 9; c++)
            {
                at(r, c).pairColor = -1;
            }
        }
        at(row, col).pairColor = 0;
    }
    //set color in current depth and update cand list
    else
    {
        at(row1, col1).pairColor = color;
        assert(at(row1, col1).lc() == 2);
        //update cand list if we have an intersecting color pair
        for (int r = 0; r < 9; r++)
        {
            for (int c = 0; c < 9; c++)
            {
                if (r != row1 && c != col1 && at(r, c).pairColor == (~color & 1))
                {
                    assert(at(r, col1).pairColor == -1);
                    assert(at(row1, c).pairColor == -1);
                    int lcbefore = at(r, col1).lc();
                    at(r, col1).candidates.erase(*candPair.begin());
                    at(r, col1).candidates.erase(*candPair.end());
                    int lcafter = at(r, col1).lc();
                    appendSolvStep(r, col1, "Cand pair " + cand2str(candPair) + " removed due to colored pair", lcbefore > lcafter);
                    lcbefore = at(row1, c).lc();
                    at(row1, c).candidates.erase(*candPair.begin());
                    at(row1, c).candidates.erase(*candPair.end());
                    lcafter = at(row1, c).lc();
                    appendSolvStep(row1, c, "Cand pair " + cand2str(candPair) + " removed due to colored pair", lcbefore > lcafter);
                }
            }
        }
    }
    //do the recursive coloring process, build a three way recursion and loop over cols, rows and blocks
    Cell* currCell = &at(row, col);
    if (row1 != -1 && col1 != -1)
    {
        currCell = &at(row1, col1);
    }
    int nextColor = ~color & 1;
    for (int idx = 0; idx < 9; idx++)
    {
        Cell* nextCell = &at(currCell->row, idx);
        if (nextCell->pairColor == -1 && nextCell->candidates == candPair && nextCell->blk != currCell->blk)
        {
            checkForIntersectingColorPairs(row, col, nextCell->row, nextCell->col, nextColor);
        }
        nextCell = &at(idx, currCell->col);
        if (nextCell->pairColor == -1 && nextCell->candidates == candPair && nextCell->blk != currCell->blk)
        {
            checkForIntersectingColorPairs(row, col, nextCell->row, nextCell->col, nextColor);
        }
        nextCell = &atBlock(currCell->blk, idx);
        if (nextCell->pairColor == -1 && nextCell->candidates == candPair)
        {
            checkForIntersectingColorPairs(row, col, nextCell->row, nextCell->col, nextColor);
        }
    }
}

void SudokuBoard::applyStrategies()
{
    for (int row = 0; row < 9; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            if (at(row, col).isGap() == false)
            {
                assert(at(row, col).lc() == 0);
                continue;
            }
            if (checkCellForNakedSingle(row, col)) continue;
            if (checkCellForHiddenSingle(row, col)) continue;
            if (checkCellForNakedPair(row, col)) continue;
            if (checkCellForHiddenPair(row, col)) continue;
            if (checkCellForNakedTriplet(row, col)) continue;
            if (checkCellForLockedCandsInBlocks(row, col)) continue;
            if (checkCellForXWing(row, col)) continue;
            if (checkCellForXYWing(row, col)) continue;
            checkForIntersectingColorPairs(row, col);
        }
    }
}
    
void SudokuBoard::solve(int numIterations)
{
    //fill each cand list
    collectCands();
    //try to solve
    int it_count = 0;
    bool valid = false;
    while (!valid && (it_count <= numIterations))
    {
        it_count++;
        applyStrategies();
        valid = this->valid();
    }
}