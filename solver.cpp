#include "solver.h"
#include <cassert>
#include <iostream>

using namespace std;


bool CandSet::remove(CandSet& set)
{
    int lcbefore = this->size();
    for (auto it = set.begin(); it != set.end(); ++it)
    {
        assert((*it > 0) && (*it < 10));
        this->data.erase(*it);
    }
    int lcafter = this->size();
    return lcbefore > lcafter;
}

string CandSet::cand2str()
{
    if (this->data.empty())
    {
        return "{}";
    }
    string res = "{";
    auto ci = this->data.begin();
    for (int k = 0; k < data.size(); k++)
    {
        res += to_string(*ci);
        if (k != data.size() - 1)
        {
            res += ", ";
            advance(ci, 1);
        }
        else
        {
            res += "}";
        }
    }
    return res;
}

CandSet CandSet::operator-(CandSet& op)
{
    CandSet dummy;
    set_difference(this->data.begin(), this->data.end(), op.data.begin(), op.data.end(), inserter(dummy.data, dummy.data.begin()));
    return dummy;
}

CandSet CandSet::operator&&(CandSet& op)
{
    CandSet dummy;
    set_intersection(this->data.begin(), this->data.end(), op.data.begin(), op.data.end(), inserter(dummy.data, dummy.data.begin()));
    return dummy;
}

CandSet CandSet::operator||(CandSet& op)
{
    CandSet dummy;
    dummy.data.insert(this->data.begin(), this->data.end());
    dummy.data.insert(op.data.begin(), op.data.end());
    return dummy;
}

CandSet& CandSet::operator+=(CandSet& op)
{
    this->data.insert(op.data.begin(), op.data.end());
    return *this;
}

CandSet& CandSet::operator-=(CandSet& op)
{
    CandSet dummy = *this - op;
    *this = dummy;
    return *this;
}

CandSet& CandSet::operator=(const CandSet& op)
{
    this->data = op.data;
    return *this;
}


void Cell::init(int idx, int digit)
{
    val = digit;
    row = idx / 9;
    col = idx % 9;
    blk = 3 * (row / 3) + (col / 3);
    blkidx = 3 * (row % 3) + (col % 3);
    rowBlkPos = 3 * (row / 3);
    colBlkPos = 3 * (col / 3);
    pairColor = -1;
}

string Cell::cord2str()
{
    return "(" + to_string(row) + ", " + to_string(col) + ")";
}


SudokuBoard::SudokuBoard(int* board)
{
    for (int i = 0; i < 81; i++)
    {
        b.at(i).init(i, board[i]);
    }
}

void SudokuBoard::appendSolvStep(int row, int col, string text, bool bReducedCands)
{
    if (bReducedCands)
    {
        pair<int, string> p = make_pair(9 * row + col, text);
        solvingSteps.push_back(p);
        //add latex code
        latexCode += "\\section{" + text + "}\n";
        latexCode += "\\sudoku{\n";
        for (int idx = 0; idx < 81; idx++)
        {
            if (idx % 9 == 0) latexCode += "    ";
            if (9 * row + col == idx && b[idx].lc() == 0) latexCode += "\\valg{" + to_string(b[idx].val) + "}";
            if (9 * row + col != idx && b[idx].lc() == 0) latexCode += "\\val{" + to_string(b[idx].val) + "}";
            if (9 * row + col == idx && b[idx].lc() > 0) latexCode += "\\candg" + b[idx].candidates.cand2str();
            if (9 * row + col != idx && b[idx].lc() > 0) latexCode += "\\cand" + b[idx].candidates.cand2str();
            if (idx % 9 < 8) latexCode += ", ";
            if (idx % 9 == 8 && idx / 9 < 8) latexCode += ",\n";
        }
        latexCode += "\n}\n";
    }
}

void SudokuBoard::copyBoard(const array<Cell, 81>& src, array<Cell, 81>& dest)
{
    for (int idx = 0; idx < 81; idx++)
    {
        dest[idx].val        = src[idx].val;
        dest[idx].row        = src[idx].row;
        dest[idx].col        = src[idx].col;
        dest[idx].blk        = src[idx].blk;
        dest[idx].blkidx     = src[idx].blkidx;
        dest[idx].rowBlkPos  = src[idx].rowBlkPos;
        dest[idx].colBlkPos  = src[idx].colBlkPos;
        dest[idx].pairColor  = src[idx].pairColor;
        dest[idx].candidates = src[idx].candidates;
    }
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

string& SudokuBoard::printSolvingSteps()
{
    for (int k = 0; k < solvingSteps.size(); k++)
    {
        int row = get<0>(solvingSteps.at(k)) / 9;
        int col = get<0>(solvingSteps.at(k)) % 9;
        string text = get<1>(solvingSteps.at(k));
        cout << "(" << to_string(row) << ", " << to_string(col) << "): " << text << endl;
    }
    return latexCode;
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

bool SudokuBoard::isSolved()
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

bool SudokuBoard::updateCandsInRow(int row, vector<int> excludedPositions, CandSet digits)
{
    bool couldReduce = false;
    for (int j = 0; j < 9; j++)
    {
        if (!(find(excludedPositions.begin(), excludedPositions.end(), j) != excludedPositions.end()))
        {
            couldReduce |= at(row, j).candidates.remove(digits);
        }
    }
    return couldReduce;
}

bool SudokuBoard::updateCandsInCol(int col, vector<int> excludedPositions, CandSet digits)
{
    bool couldReduce = false;
    for (int i = 0; i < 9; i++)
    {
        if (!(find(excludedPositions.begin(), excludedPositions.end(), i) != excludedPositions.end()))
        {
            couldReduce |= at(i, col).candidates.remove(digits);
        }
    }
    return couldReduce;
}

bool SudokuBoard::updateCandsInBlock(int blk, vector<int> excludedPositions, CandSet digits)
{
    bool couldReduce = false;
    for (int idx = 0; idx < 9; idx++)
    {
        if (!(find(excludedPositions.begin(), excludedPositions.end(), idx) != excludedPositions.end()))
        {
            couldReduce |= atBlock(blk, idx).candidates.remove(digits);
        }
    }
    return couldReduce;
}

void SudokuBoard::setFinalValue(int row, int col)
{
    assert(at(row, col).lc() == 1);
    assert(at(row, col).isGap());
    int newDigit = *at(row, col).candidates.begin();
    assert(newDigit > 0 && newDigit < 10);
    //update cand list in same row
    updateCandsInRow(row, vector<int>{col}, at(row, col).candidates);
    //update cand list in same col
    updateCandsInCol(col, vector<int>{row}, at(row, col).candidates);
    //update cand list in same block
    updateCandsInBlock(at(row, col).blk, vector<int>{at(row, col).blkidx}, at(row, col).candidates);
    at(row, col).val = newDigit;
    at(row, col).candidates.erase(newDigit);
}

bool SudokuBoard::checkCellForNakedSingle(int row, int col, string prefix)
{
    if (at(row, col).lc() != 1)
    {
        return false;
    }
    setFinalValue(row, col);
    appendSolvStep(row, col, prefix + "Naked Single", true);
    return true;
}

bool SudokuBoard::checkCellForHiddenSingle(int row, int col, string prefix)
{
    if (at(row, col).lc() < 2)
    {
        return false;
    }
    //search along row and collect all other candidates
    CandSet allOtherCands, t;
    for (int c = 0; c < 9; c++)
    {
        if (c == col || !at(row, c).isGap()) continue;
        allOtherCands += at(row, c).candidates;
    }
    t = at(row, col).candidates - allOtherCands;
    if (t.size() == 1)
    {
        at(row, col).candidates -= allOtherCands;
        setFinalValue(row, col);
        appendSolvStep(row, col, prefix + "Hidden Single", true);
        return true;
    }
    //search along col and collect all other candidates
    allOtherCands.clear();
    for (int r = 0; r < 9; r++)
    {
        if (r == row || !at(r, col).isGap()) continue;
        allOtherCands += at(r, col).candidates;
    }
    t = at(row, col).candidates - allOtherCands;
    if (t.size() == 1)
    {
        at(row, col).candidates -= allOtherCands;
        setFinalValue(row, col);
        appendSolvStep(row, col, prefix + "Hidden Single", true);
        return true;
    }
    //search within block and collect all other candidates
    allOtherCands.clear();
    int blk = at(row, col).blk;
    for (int blkIdx = 0; blkIdx < 9; blkIdx++)
    {
        if (blkIdx == at(row, col).blkidx || !atBlock(blk, blkIdx).isGap()) continue;
        allOtherCands += atBlock(blk, blkIdx).candidates;
    }
    t = at(row, col).candidates - allOtherCands;
    if (t.size() == 1)
    {
        at(row, col).candidates -= allOtherCands;
        setFinalValue(row, col);
        appendSolvStep(row, col, prefix + "Hidden Single", true);
        return true;
    }
    return false;
}

bool SudokuBoard::checkCellForNakedPair(int row, int col, string prefix)
{
    if (at(row, col).lc() != 2)
    {
        return false;
    }
    CandSet candPair = b[9 * row + col].candidates;
    for (int idx = 0; idx < 9; idx++)
    {
        //if cell(row, col) can basically be a naked pair, look for the other part in same row
        int c = idx;
        if (c != col && at(row, c).candidates == candPair)
        {
            bool stepReducedCands = updateCandsInRow(row, vector<int>{col, c}, candPair);
            appendSolvStep(row, col, prefix + "Naked Pair with cell " + at(row, c).cord2str(), stepReducedCands);
            if (stepReducedCands) return true;
        }
        //if cell(row, col) can basically be a naked pair, look for the other part in same col
        int r = idx;
        if (r != row && at(r, col).candidates == candPair)
        {
            bool stepReducedCands = updateCandsInCol(col, vector<int>{row, r}, candPair);
            appendSolvStep(row, col, prefix + "Naked Pair with cell " + at(r, col).cord2str(), stepReducedCands);
            if (stepReducedCands) return true;
        }
        //if cell(row, col) can basically be a naked pair, look for the other part in same block
        int blkIdx = idx;
        int blk = at(row, col).blk;
        if (blkIdx != at(row, col).blkidx && atBlock(blk, blkIdx).candidates == candPair)
        {
            bool stepReducedCands = updateCandsInBlock(blk, vector<int>{at(row, col).blkidx, blkIdx}, candPair);
            appendSolvStep(row, col, prefix + "Naked Pair with cell " + atBlock(blk, blkIdx).cord2str(), stepReducedCands);
            if (stepReducedCands) return true;
        }
    }
    return false;
}

bool SudokuBoard::checkCellForHiddenPair(int row, int col, string prefix)
{
    if (at(row, col).lc() < 3)
    {
        return false;
    }
    CandSet allOtherCands, t;
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
                allOtherCands += at(row, j).candidates;
            }
            t = at(row, col).candidates && at(row, c).candidates;
            t = t - allOtherCands;
            if (t.size() == 2)
            {
                at(row, col).candidates = t;
                at(row, c).candidates = t;
                appendSolvStep(row, col, prefix + "Hidden Pair " + t.cand2str() + " with cell " + at(row, c).cord2str(), true);
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
                allOtherCands += at(i, col).candidates;
            }
            t = at(row, col).candidates && at(r, col).candidates;
            t = t - allOtherCands;
            if (t.size() == 2)
            {
                at(row, col).candidates = t;
                at(r, col).candidates = t;
                appendSolvStep(row, col, prefix + "Hidden Pair " + t.cand2str() + " with cell " + at(r, col).cord2str(), true);
                return true;
            }
        }
        //search the pair within block
        int blkIdx = idx;
        int blk = at(row, col).blk;
        if (blkIdx != at(row, col).blkidx && atBlock(blk, blkIdx).isGap())
        {
            //collect all other candidates
            allOtherCands.clear();
            for (int bi = 0; bi < 9; bi++)
            {
                if (bi == at(row, col).blkidx || bi == blkIdx) continue;
                allOtherCands += atBlock(blk, bi).candidates;
            }
            t = at(row, col).candidates && atBlock(blk, blkIdx).candidates;
            t = t - allOtherCands;
            if (t.size() == 2)
            {
                at(row, col).candidates = t;
                atBlock(blk, blkIdx).candidates = t;
                appendSolvStep(row, col, prefix + "Hidden Pair " + t.cand2str() + " with cell " + atBlock(blk, blkIdx).cord2str(), true);
                return true;
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForNakedTriplet(int row, int col, string prefix)
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
                CandSet u = at(row, col).candidates;
                u += at(row, c0).candidates;
                u += at(row, c1).candidates;
                if (u.size() == 3)
                {
                    bool stepReducedCands = updateCandsInRow(row, vector<int>{col, c0, c1}, u);
                    appendSolvStep(row, col, prefix + "Naked Triplet in row with cell " + at(row, c0).cord2str() + " and cell " + at(row, c1).cord2str(), stepReducedCands);
                    if (stepReducedCands) return true;
                }
            }
            //if cell(row, col) can basically be a naked triple, look for the other part in same col
            int r0 = idx0;
            int r1 = idx1;
            if (r0 != row && r1 != row && r0 != r1 && (at(r0, col).lc() > 1) && (at(r1, col).lc() > 1))
            {
                CandSet u = at(row, col).candidates;
                u += at(r0, col).candidates;
                u += at(r1, col).candidates;
                if (u.size() == 3)
                {
                    bool stepReducedCands = updateCandsInCol(col, vector<int>{row, r0, r1}, u);
                    appendSolvStep(row, col, prefix + "Naked Triplet in col with cell " + at(r0, col).cord2str() + " and cell " + at(r1, col).cord2str(), stepReducedCands);
                    if (stepReducedCands) return true;
                }
            }
            //if cell(row, col) can basically be a naked triple, look for the other part in same block
            int blkIdx0 = idx0;
            int blkIdx1 = idx1;
            int blk = at(row, col).blk;
            if (blkIdx0 != at(row, col).blkidx && blkIdx1 != at(row, col).blkidx && blkIdx0 != blkIdx1 && atBlock(blk, blkIdx0).lc() > 1 && atBlock(blk, blkIdx1).lc() > 1)
            {
                CandSet u = at(row, col).candidates;
                u += atBlock(blk, blkIdx0).candidates;
                u += atBlock(blk, blkIdx1).candidates;
                if (u.size() == 3)
                {
                    bool stepReducedCands = updateCandsInBlock(blk, vector<int>{at(row, col).blkidx, blkIdx0, blkIdx1}, u);
                    appendSolvStep(row, col, prefix + "Naked Triplet in block with cell " + atBlock(blk, blkIdx0).cord2str() + " and cell " + atBlock(blk, blkIdx1).cord2str(), stepReducedCands);
                    if (stepReducedCands) return true;
                }
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForXWing(int row, int col, string prefix)
{
    if (!at(row, col).isGap())
    {
        return false;
    }
    CandSet allOtherCands, t, commonC;
    for (int r = row + 1; r < 9; r++)
    {
        for (int c = col + 1; c < 9; c++)
        {
            if (!(at(row, c).isGap() && at(r, col).isGap() && at(r, c).isGap())) continue;
            commonC = at(row, col).candidates && at(row, c).candidates && at(r, col).candidates && at(r, c).candidates;
            //check if the x pattern has a common candidate
            if (commonC.size() == 0) continue;
            //check conditions for x-wing column-wise
            allOtherCands.clear();
            for (int j = 0; j < 9; j++)
            {
                if (j != col && j != c)
                {
                    allOtherCands += at(row, j).candidates;
                    allOtherCands += at(r, j).candidates;
                }
            }
            t = commonC - allOtherCands;
            bool stepReducedCands = false;
            if (t.size() == 1)
            {
                stepReducedCands |= updateCandsInCol(col, vector<int>{row, r}, t);
                stepReducedCands |= updateCandsInCol(c, vector<int>{row, r}, t);
                appendSolvStep(row, col, prefix + "column-wise X-Wing with diag cell" + at(r, c).cord2str(), stepReducedCands);
                if (stepReducedCands) return true;
            }
            //check conditions for x-wing row-wise
            allOtherCands.clear();
            for (int i = 0; i < 9; i++)
            {
                if (i != row && i != r)
                {
                    allOtherCands += at(i, col).candidates;
                    allOtherCands += at(i, c).candidates;
                }
            }
            t = commonC - allOtherCands;
            stepReducedCands = false;
            if (t.size() == 1)
            {
                stepReducedCands |= updateCandsInRow(row, vector<int>{col, c}, t);
                stepReducedCands |= updateCandsInRow(r, vector<int>{col, c}, t);
                appendSolvStep(row, col, prefix + "row-wise X-Wing with diag cell" + at(r, c).cord2str(), stepReducedCands);
                if (stepReducedCands) return true;
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForXYWing(int row, int col, string prefix)
{
    if (at(row, col).lc() != 2)
    {
        return false;
    }
    int blk = at(row, col).blk;
    int rowstart = at(row, col).rowBlkPos;
    int colstart = at(row, col).colBlkPos;
    CandSet t0, t1, t2;
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
                bool stepReducedCands = at(r, c).candidates.remove(t2);
                appendSolvStep(row, col, prefix + "XY wing", stepReducedCands);
                if (stepReducedCands) return true;
            }
        }
    }
    //check if we have a xy pattern in block-row scenario
    for (int bi = 0; bi < 9; bi++)
    {
        if (atBlock(blk, bi).row == row) continue;
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
                bool stepReducedCands = false;
                for (int c1 = at(row, c).colBlkPos; c1 < at(row, c).colBlkPos + 3; c1++)
                {
                    stepReducedCands |= at(r1, c1).candidates.remove(t2);
                }
                for (int c1 = colstart; c1 < colstart + 3; c1++)
                {
                    stepReducedCands |= at(row, c1).candidates.remove(t2);
                }
                appendSolvStep(row, col, prefix + "XY wing in block-row scenario", stepReducedCands);
                if (stepReducedCands) return true;
            }
        }
    }
    //check if we have a xy pattern in block-column scenario
    for (int bi = 0; bi < 9; bi++)
    {
        if (atBlock(blk, bi).col == col) continue;
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
                bool stepReducedCands = false;
                for (int r1 = at(r, col).rowBlkPos; r1 < at(r, col).rowBlkPos + 3; r1++)
                {
                    stepReducedCands |= at(r1, c1).candidates.remove(t2);
                }
                for (int r1 = rowstart; r1 < rowstart + 3; r1++)
                {
                    stepReducedCands |= at(r1, col).candidates.remove(t2);
                }
                appendSolvStep(row, col, prefix + "XY wing in block-column scenario", stepReducedCands);
                if (stepReducedCands) return true;
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForLockedCandsInBlocks(int row, int col, string prefix)
{
    if (at(row, col).lc() < 2)
    {
        return false;
    }
    int rowstart = at(row, col).rowBlkPos;
    int colstart = at(row, col).colBlkPos;
    int blk = at(row, col).blk;
    //check row-within-block scenario
    CandSet allOtherCands, lockedCands;
    for (int i = rowstart; i < rowstart + 3; i++)
    {
        for (int j = colstart; j < colstart + 3; j++)
        {
            if (i == row || !at(i, j).isGap()) continue;
            allOtherCands += at(i, j).candidates;
        }
    }
    lockedCands = at(row, col).candidates - allOtherCands;
    if (lockedCands.size() > 0)
    {
        bool stepReducedCands = updateCandsInRow(row, vector<int>{colstart, colstart + 1, colstart + 2}, lockedCands);
        appendSolvStep(row, col, prefix + "Cands " + lockedCands.cand2str() + " are locked in row", stepReducedCands);
        if (stepReducedCands) return true;
    }
    //check col-within-block scenario
    allOtherCands.clear();
    for (int i = rowstart; i < rowstart + 3; i++)
    {
        for (int j = colstart; j < colstart + 3; j++)
        {
            if (j == col || !at(i, j).isGap()) continue;
            allOtherCands += at(i, j).candidates;
        }
    }
    lockedCands = at(row, col).candidates - allOtherCands;
    if (lockedCands.size() > 0)
    {
        bool stepReducedCands = updateCandsInCol(col, vector<int>{rowstart, rowstart + 1, rowstart + 2}, lockedCands);
        appendSolvStep(row, col, prefix + "Cands " + lockedCands.cand2str() + " are locked in col", stepReducedCands);
        if (stepReducedCands) return true;
    }
    //check block-within-row scenario
    allOtherCands.clear();
    for (int j = 0; j < 9; j++)
    {
        if ((j >= colstart) && (j < colstart + 3)) continue;
        if (!at(row, j).isGap()) continue;
        allOtherCands += at(row, j).candidates;
    }
    lockedCands = at(row, col).candidates - allOtherCands;
    if (lockedCands.size() > 0)
    {
        int exBlkIdx = 3 * (at(row, col).blkidx / 3);
        bool stepReducedCands = updateCandsInBlock(blk, vector<int>{exBlkIdx, exBlkIdx + 1, exBlkIdx + 2}, lockedCands);
        appendSolvStep(row, col, prefix + "Cands " + lockedCands.cand2str() + " are locked in block-row", stepReducedCands);
        if (stepReducedCands) return true;
    }
    //check block-within-col scenario
    allOtherCands.clear();
    for (int i = 0; i < 9; i++)
    {
        if ((i >= rowstart) && (i < rowstart + 3)) continue;
        if (!at(i, col).isGap()) continue;
        allOtherCands += at(i, col).candidates;
    }
    lockedCands = at(row, col).candidates - allOtherCands;
    if (lockedCands.size() > 0)
    {
        int exBlkIdx = at(row, col).blkidx % 3;
        bool stepReducedCands = updateCandsInBlock(blk, vector<int>{exBlkIdx, exBlkIdx + 3, exBlkIdx + 6}, lockedCands);
        appendSolvStep(row, col, prefix + "Cands " + lockedCands.cand2str() + " are locked in block-column", stepReducedCands);
        if (stepReducedCands) return true;
    }
    return false;
}

bool SudokuBoard::checkForIntersectingColorPairs(int row, int col, string prefix, int row1, int col1, int color)
{
    if (at(row, col).lc() != 2)
    {
        return false;
    }
    assert(color == 0 || color == 1);
    CandSet candPair = at(row, col).candidates;
    if (row1 >= 0 && col1 >= 0)
    {
        at(row1, col1).pairColor = color;
        assert(at(row1, col1).lc() == 2);
    }
    for (int r = 0; r < 9; r++)
    {
        for (int c = 0; c < 9; c++)
        {
            //initialize recursion
            if (row1 == -1 && col1 == -1)
            {
                at(r, c).pairColor = (r == row && c == col) ? 0 : -1;
                continue;
            }
            //update cand list if we have an intersecting color pair
            if (r != row1 && c != col1 && at(r, c).pairColor == (~color & 1))
            {
                assert(at(r, col1).pairColor == -1);
                assert(at(row1, c).pairColor == -1);
                bool step1ReducedCands = at(r, col1).candidates.remove(candPair);
                appendSolvStep(r, col1, prefix + "Cand pair " + candPair.cand2str() + " removed due to colored pair", step1ReducedCands);
                bool step2ReducedCands = at(row1, c).candidates.remove(candPair);
                appendSolvStep(row1, c, prefix + "Cand pair " + candPair.cand2str() + " removed due to colored pair", step2ReducedCands);
                if (step1ReducedCands || step2ReducedCands) return true;
            }
            //determine candPair by a Naked Pair that has only one candidate in common
            if (r != row1 && c != col1 && at(r, c).pairColor == color)
            {
                for (int i = 0; i < 9; i++)
                {
                    if (i == row1 || i == r) continue;
                    CandSet t0 = at(i, col1).candidates;
                    CandSet t1 = at(i, c).candidates;
                    CandSet t2 = candPair - t0;
                    if (t0.size() == 2 && t0 == t1 && t2.size() == 1)
                    {
                        at(row1, col1).candidates = t2;
                        at(r, c).candidates = t2;
                        appendSolvStep(row1, col1, prefix + "Cand pair " + candPair.cand2str() + " set due to pair in " + at(i, col1).cord2str() + " and " + at(i, c).cord2str(), true);
                        appendSolvStep(r, c, prefix + "Cand pair " + candPair.cand2str() + " set due to pair in " + at(i, col1).cord2str() + " and " + at(i, c).cord2str(), true);
                        return true;
                    }
                }
                for (int j = 0; j < 9; j++)
                {
                    if (j == col1 || j == c) continue;
                    CandSet t0 = at(row1, j).candidates;
                    CandSet t1 = at(r, j).candidates;
                    CandSet t2 = candPair - t0;
                    if (t0.size() == 2 && t0 == t1 && t2.size() == 1)
                    {
                        at(row1, col1).candidates = t2;
                        at(r, c).candidates = t2;
                        appendSolvStep(row1, col1, prefix + "Cand pair " + candPair.cand2str() + " set due to pair in " + at(row1, j).cord2str() + " and " + at(r, j).cord2str(), true);
                        appendSolvStep(r, c, prefix + "Cand pair " + candPair.cand2str() + " set due to pair in " + at(row1, j).cord2str() + " and " + at(r, j).cord2str(), true);
                        return true;
                    }
                }
            }
        }
    }
    //start the recursive coloring process and build the color tree
    Cell* currCell = &at(row, col);
    if (row1 != -1 && col1 != -1)
    {
        currCell = &at(row1, col1);
    }
    int nextColor = ~color & 1;
    //check if one cell color in a block has a relation to a cell which is either in same col or in same row
    int numLeftCellsInBlk = 0;
    int relRow = -1;
    int relCol = -1;
    for (int bi = 0; bi < 9; bi++)
    {
        if (bi == currCell->blkidx) continue;
        if (atBlock(currCell->blk, bi).isGap())
        {
            numLeftCellsInBlk++;
            if (numLeftCellsInBlk == 1)
            {
                relRow = atBlock(currCell->blk, bi).row;
                relCol = atBlock(currCell->blk, bi).col;
            }
            if (numLeftCellsInBlk > 1 && relRow != atBlock(currCell->blk, bi).row) relRow = -1;
            if (numLeftCellsInBlk > 1 && relCol != atBlock(currCell->blk, bi).col) relCol = -1;
        }
    }
    //recursive calls
    for (int idx = 0; idx < 9; idx++)
    {
        Cell* nextCell = &at(currCell->row, idx);
        if (nextCell->pairColor == -1 && nextCell->candidates == candPair && nextCell->blk != currCell->blk)
        {
            return checkForIntersectingColorPairs(row, col, prefix, nextCell->row, nextCell->col, nextColor);
        }
        nextCell = &at(idx, currCell->col);
        if (nextCell->pairColor == -1 && nextCell->candidates == candPair && nextCell->blk != currCell->blk)
        {
            return checkForIntersectingColorPairs(row, col, prefix, nextCell->row, nextCell->col, nextColor);
        }
        nextCell = &atBlock(currCell->blk, idx);
        if (nextCell->pairColor == -1 && nextCell->candidates == candPair)
        {
            return checkForIntersectingColorPairs(row, col, prefix, nextCell->row, nextCell->col, nextColor);
        }
        if (numLeftCellsInBlk > 1 && relCol > -1 && !(idx >= currCell->rowBlkPos && idx < currCell->rowBlkPos + 3))
        {
            nextCell = &at(idx, relCol);
            if (nextCell->pairColor == -1 && nextCell->candidates == candPair)
            {
                return checkForIntersectingColorPairs(row, col, prefix, nextCell->row, nextCell->col, color);
            }
        }
        if (numLeftCellsInBlk > 1 && relRow > -1 && !(idx >= currCell->colBlkPos && idx < currCell->colBlkPos + 3))
        {
            nextCell = &at(relRow, idx);
            if (nextCell->pairColor == -1 && nextCell->candidates == candPair)
            {
                return checkForIntersectingColorPairs(row, col, prefix, nextCell->row, nextCell->col, color);
            }
        }
    }
    return false;
}

void SudokuBoard::applyStrategies(string prefix)
{
    for (int row = 0; row < 9; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            if (at(row, col).lc() == 0)
            {
                //step over if there are no candidates
                continue;
            }
            if (checkCellForNakedSingle(row, col, prefix)) continue;
            if (checkCellForHiddenSingle(row, col, prefix)) continue;
            bool tryNext = !checkCellForNakedPair(row, col, prefix);
            if (tryNext) tryNext = !checkCellForHiddenPair(row, col, prefix);
            if (tryNext) tryNext = !checkCellForLockedCandsInBlocks(row, col, prefix);
            if (tryNext) tryNext = !checkCellForNakedTriplet(row, col, prefix);
            if (tryNext) tryNext = !checkCellForXWing(row, col, prefix);
            if (tryNext) tryNext = !checkCellForXYWing(row, col, prefix);
            if (tryNext) checkForIntersectingColorPairs(row, col, prefix);
        }
    }
}

bool SudokuBoard::tryForcingChain(int numIterations)
{
    array<Cell, 81> origBoard;
    copyBoard(b, origBoard);
    string origLatexCode = latexCode;
    int it_count;
    for (int r = 0; r < 9; r++)
    {
        for (int c = 0; c < 9; c++)
        {
            if (origBoard[9 * r + c].lc() != 2)
            {
                continue;
            }
            bool bInvalid0 = false;
            bool bInvalid1 = false;
            const int rmCand0 = *origBoard[9 * r + c].candidates.begin();
            const int rmCand1 = *next(origBoard[9 * r + c].candidates.begin());
            //use first candidate as solution
            copyBoard(origBoard, b);
            latexCode = origLatexCode;
            at(r, c).candidates.erase(rmCand0);
            setFinalValue(r, c);
            it_count = 0;
            while (!bInvalid0 && (it_count <= numIterations))
            {
                it_count++;
                applyStrategies("Remove cand " + to_string(rmCand0) + " from cell " + at(r, c).cord2str() + ": ");
                for (int i = 0; i < 81; i++)
                {
                    bInvalid0 |= (b.at(i).isGap() && b.at(i).lc() == 0);
                }
            }
            bool bSolved0 = isSolved();
            string latexCode0 = latexCode;
            //use second candidate as solution
            copyBoard(origBoard, b);
            latexCode = origLatexCode;
            at(r, c).candidates.erase(rmCand1);
            setFinalValue(r, c);
            it_count = 0;
            while (!bInvalid1 && (it_count <= numIterations))
            {
                it_count++;
                applyStrategies("Remove cand " + to_string(rmCand1) + " from cell " + at(r, c).cord2str() + ": ");
                for (int i = 0; i < 81; i++)
                {
                    bInvalid1 |= (b.at(i).isGap() && b.at(i).lc() == 0);
                }
            }
            bool bSolved1 = isSolved();
            string latexCode1 = latexCode;
            //check both results
            if ((bInvalid0 && bSolved1) || (bInvalid1 && bSolved0))
            {
                copyBoard(origBoard, b);
                latexCode = bSolved0 ? latexCode1 : latexCode0;
                at(r, c).candidates.erase(bSolved0 ? rmCand0 : rmCand1);
                setFinalValue(r, c);
                appendSolvStep(r, c, "Forcing chain", true);
                while (!isSolved())
                {
                    applyStrategies();
                }
                return true;
            }
        }
    }
    return false;
}
    
bool SudokuBoard::solve(int numIterations)
{
    //fill each cand list
    collectCands();
    //try to solve
    int it_count = 0;
    bool bIsSolved = false;
    latexCode = "";
    while (!bIsSolved && (it_count <= numIterations))
    {
        it_count++;
        applyStrategies();
        bIsSolved = isSolved();
    }
    //forcing chain
    if (!bIsSolved)
    {
        bIsSolved = tryForcingChain(numIterations);
    }
    //add backslashes in Latex code
    size_t n = 0;
    while ((n = latexCode.find(" {", n)) != string::npos)
    {
        latexCode.replace(n, 2, " \\{");
        n += 3;
    }
    n = 0;
    while ((n = latexCode.find("} ", n)) != string::npos)
    {
        latexCode.replace(n, 2, "\\} ");
        n += 3;
    }
    return bIsSolved;
}