#include "solver.h"
#include <cassert>
#include <iostream>
#include <sstream>

using namespace std;


bool CandSet::remove(CandSet set)
{
    int lcbefore = this->size();
    for (auto it = set.begin(); it != set.end(); ++it)
    {
        assert((*it > 0) && (*it < 10));
        data.erase(*it);
    }
    int lcafter = this->size();
    return lcbefore > lcafter;
}

string CandSet::to_string()
{
    ostringstream stream;
    copy(data.begin(), data.end(), ostream_iterator<int>(stream, ","));
    string res = stream.str();
    res.pop_back();
    return res;
}

bool CandSet::operator==(CandSet& op)
{
    return this->data == op.data;
}

bool CandSet::operator!=(CandSet& op)
{
    return this->data != op.data;
}

CandSet CandSet::operator-(CandSet op)
{
    CandSet dummy;
    set_difference(data.begin(), data.end(), op.data.begin(), op.data.end(), inserter(dummy.data, dummy.data.begin()));
    return dummy;
}

CandSet CandSet::operator&&(CandSet op)
{
    CandSet dummy;
    set_intersection(data.begin(), data.end(), op.data.begin(), op.data.end(), inserter(dummy.data, dummy.data.begin()));
    return dummy;
}

CandSet CandSet::operator+(CandSet op)
{
    CandSet dummy;
    dummy.data.insert(data.begin(), data.end());
    dummy.data.insert(op.data.begin(), op.data.end());
    return dummy;
}

CandSet& CandSet::operator+=(CandSet op)
{
    data.insert(op.data.begin(), op.data.end());
    return *this;
}

CandSet& CandSet::operator-=(CandSet op)
{
    *this = *this - op;
    return *this;
}

CandSet& CandSet::operator=(const CandSet op)
{
    data = op.data;
    return *this;
}


Cell Cell::operator=(const Cell& other)
{
    this->val        = other.val;
    this->row        = other.row;
    this->col        = other.col;
    this->blk        = other.blk;
    this->blkidx     = other.blkidx;
    this->rowBlkPos  = other.rowBlkPos;
    this->colBlkPos  = other.colBlkPos;
    this->pairColor  = other.pairColor;
    this->candidates = other.candidates;
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

string Cell::cand2str()
{
    if (candidates.size() == 0)
    {
        return "{}";
    }
    return "{" + candidates.to_string() + "}";
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
    if (!bReducedCands)
    {
        return;
    }
    pair<int, string> p = make_pair(9 * row + col, text);
    solvingSteps.push_back(p);
    //add latex code
    latexCode += "\\section{" + text + "}\n" + "\\sudoku{\n";
    for (Cell& cell : b)
    {
        if (cell.col == 0) latexCode += "    ";
        if (row == cell.row && col == cell.col)
        {
            if (cell.lc() == 0) latexCode += "\\valg{" + to_string(cell.val) + "}";
            if (cell.lc() > 0) latexCode += "\\candg" + cell.cand2str();
        }
        else
        {
            if (cell.lc() == 0) latexCode += "\\val{" + to_string(cell.val) + "}";
            if (cell.lc() > 0) latexCode += "\\cand" + cell.cand2str();
        }
        if (cell.col < 8) latexCode += ", ";
        if (cell.col == 8 && cell.row < 8) latexCode += ",\n";
    }
    latexCode += "\n}\n";
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
        i++;
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
        i++;
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
        i++;
    }
    return found;
}

bool SudokuBoard::isValid()
{
    for (Cell& cell : b)
    {
        if (cell.isGap() && cell.lc() == 0) return false;

        if (cell.isGap()) continue;
        int val = cell.val;
        cell.val = 0;
        if (isInBlock(cell.blk, val) || isInCol(cell.col, val) || isInRow(cell.row, val))
        {
            return false;
        }
        cell.val = val;
    }
    return true;
}

bool SudokuBoard::isSolved()
{
    for (Cell& cell : b)
    {
        if (cell.isGap()) return false;
    }
    return isValid();
}

void SudokuBoard::fillCands()
{
    for (Cell& cell : b)
    {
        if (!cell.isGap()) continue;
        for (int dig = 1; dig < 10; dig++)
        {
            if (!(isInRow(cell.row, dig) || isInCol(cell.col, dig) || isInBlock(cell.blk, dig)))
            {
                cell.candidates.insert(dig);
            }
        }
    }
}

CandSet SudokuBoard::collectCands(Cell& cell, const char mode_r_c_b, vector<int> exclude)
{
    CandSet _union;
    for (int i = 0; i < 9; i++)
    {
        if (find(exclude.begin(), exclude.end(), i) != exclude.end())
        {
            continue;
        }
        if (mode_r_c_b == 'r')
        {
            _union = _union + at(cell.row, i).candidates;
        }
        else if (mode_r_c_b == 'c')
        {
            _union = _union + at(i, cell.col).candidates;
        }
        else if (mode_r_c_b == 'b')
        {
            _union = _union + atBlock(cell.blk, i).candidates;
        }
    }
    return _union;
}

bool SudokuBoard::updateCands(CandSet digits, Cell& cell, const char mode_r_c_b, vector<int> exclude)
{
    bool couldReduce = false;
    for (int idx = 0; idx < 9; idx++)
    {
        if (find(exclude.begin(), exclude.end(), idx) != exclude.end())
        {
            continue;
        }
        if (mode_r_c_b == 'r')
        {
            couldReduce |= at(cell.row, idx).candidates.remove(digits);
        }
        else if (mode_r_c_b == 'c')
        {
            couldReduce |= at(idx, cell.col).candidates.remove(digits);
        }
        else if (mode_r_c_b == 'b')
        {
            couldReduce |= atBlock(cell.blk, idx).candidates.remove(digits);
        }
    }
    return couldReduce;
}

void SudokuBoard::setFinalValue(Cell& cell)
{
    assert(cell.lc() == 1);
    assert(cell.isGap());
    int newDigit = cell.candidates.getCand(0);
    assert(newDigit > 0 && newDigit < 10);
    //update cand list in same row, in same col and block
    updateCands(cell.candidates, cell, 'r', vector<int>{cell.col});
    updateCands(cell.candidates, cell, 'c', vector<int>{cell.row});
    updateCands(cell.candidates, cell, 'b', vector<int>{cell.blkidx});
    //set digit and delete candidate
    cell.val = newDigit;
    cell.candidates.erase(newDigit);
}

bool SudokuBoard::checkCellForNakedSingle(Cell& cell, string prefix)
{
    if (cell.lc() != 1)
    {
        return false;
    }
    setFinalValue(cell);
    appendSolvStep(cell.row, cell.col, prefix + "Naked Single", true);
    return true;
}

bool SudokuBoard::checkCellForHiddenSingle(Cell& cell, string prefix)
{
    if (cell.lc() < 2)
    {
        return false;
    }
    CandSet uniqueC;
    //search along row and collect all other candidates
    uniqueC = cell.candidates - collectCands(cell, 'r', vector<int>{cell.col});
    if (uniqueC.size() == 1)
    {
        cell.candidates = uniqueC;
        setFinalValue(cell);
        appendSolvStep(cell.row, cell.col, prefix + "Hidden Single", true);
        return true;
    }
    //search along col and collect all other candidates
    uniqueC = cell.candidates - collectCands(cell, 'c', vector<int>{cell.row});
    if (uniqueC.size() == 1)
    {
        cell.candidates = uniqueC;
        setFinalValue(cell);
        appendSolvStep(cell.row, cell.col, prefix + "Hidden Single", true);
        return true;
    }
    //search within block and collect all other candidates
    uniqueC = cell.candidates - collectCands(cell, 'b', vector<int>{cell.blkidx});
    if (uniqueC.size() == 1)
    {
        cell.candidates = uniqueC;
        setFinalValue(cell);
        appendSolvStep(cell.row, cell.col, prefix + "Hidden Single", true);
        return true;
    }
    return false;
}

bool SudokuBoard::checkCellForNakedPair(Cell& cell, string prefix)
{
    if (cell.lc() != 2)
    {
        return false;
    }
    CandSet candPair = cell.candidates;
    for (int idx = 0; idx < 9; idx++)
    {
        //if cell(row, col) can basically be a naked pair, look for the other part in same row
        Cell* cell1 = &at(cell.row, idx);
        if (cell1->col != cell.col && cell1->candidates == candPair)
        {
            bool stepReducedCands = updateCands(candPair, cell, 'r', vector<int>{cell.col, cell1->col});
            appendSolvStep(cell.row, cell.col, prefix + "Naked Pair with cell " + cell1->cord2str(), stepReducedCands);
            if (stepReducedCands) return true;
        }
        //if cell(row, col) can basically be a naked pair, look for the other part in same col
        cell1 = &at(idx, cell.col);
        if (cell1->row != cell.row && cell1->candidates == candPair)
        {
            bool stepReducedCands = updateCands(candPair, cell, 'c', vector<int>{cell.row, cell1->row});
            appendSolvStep(cell.row, cell.col, prefix + "Naked Pair with cell " + cell1->cord2str(), stepReducedCands);
            if (stepReducedCands) return true;
        }
        //if cell(row, col) can basically be a naked pair, look for the other part in same block
        cell1 = &atBlock(cell.blk, idx);
        if (cell1->blkidx != cell.blkidx && cell1->candidates == candPair)
        {
            bool stepReducedCands = updateCands(candPair, cell, 'b', vector<int>{cell.blkidx, cell1->blkidx});
            appendSolvStep(cell.row, cell.col, prefix + "Naked Pair with cell " + cell1->cord2str(), stepReducedCands);
            if (stepReducedCands) return true;
        }
    }
    return false;
}

bool SudokuBoard::checkCellForHiddenPair(Cell& cell, string prefix)
{
    if (cell.lc() < 3)
    {
        return false;
    }
    CandSet uniqueC;
    for (int idx = 0; idx < 9; idx++)
    {
        //search for a pair along row
        Cell* cell1 = &at(cell.row, idx);
        if (cell1->col != cell.col && cell1->isGap())
        {
            uniqueC = (cell.candidates && cell1->candidates) - collectCands(cell, 'r', vector<int>{cell.col, cell1->col});
            if (uniqueC.size() == 2)
            {
                cell.candidates = uniqueC;
                cell1->candidates = uniqueC;
                appendSolvStep(cell.row, cell.col, prefix + "Hidden Pair " + cell.cand2str() + " with cell " + cell1->cord2str(), true);
                return true;
            }
        }
        //search for a pair along col
        cell1 = &at(idx, cell.col);
        if (cell1->row != cell.row && cell1->isGap())
        {
            uniqueC = (cell.candidates && cell1->candidates) - collectCands(cell, 'c', vector<int>{cell.row, cell1->row});
            if (uniqueC.size() == 2)
            {
                cell.candidates = uniqueC;
                cell1->candidates = uniqueC;
                appendSolvStep(cell.row, cell.col, prefix + "Hidden Pair " + cell.cand2str() + " with cell " + cell1->cord2str(), true);
                return true;
            }
        }
        //search the pair within block
        cell1 = &atBlock(cell.blk, idx);
        if (cell1->blkidx != cell.blkidx && cell1->isGap())
        {
            uniqueC = (cell.candidates && cell1->candidates) - collectCands(cell, 'b', vector<int>{cell.blkidx, cell1->blkidx});
            if (uniqueC.size() == 2)
            {
                cell.candidates = uniqueC;
                cell1->candidates = uniqueC;
                appendSolvStep(cell.row, cell.col, prefix + "Hidden Pair " + cell.cand2str() + " with cell " + cell1->cord2str(), true);
                return true;
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForNakedTriplet(Cell& cell, string prefix)
{
    if (cell.lc() != 2 && cell.lc() != 3)
    {
        return false;
    }
    for (int idx0 = 0; idx0 < 9; idx0++)
    {
        for (int idx1 = 0; idx1 < 9; idx1++)
        {
            //if cell(row, col) can basically be a naked triple, look for the other part in same row
            Cell* cell1 = &at(cell.row, idx0);
            Cell* cell2 = &at(cell.row, idx1);
            if (idx0 != idx1 && cell1->col != cell.col && cell2->col != cell.col && cell1->lc() > 1 && cell2->lc() > 1)
            {
                CandSet u = cell.candidates + cell1->candidates + cell2->candidates;
                if (u.size() == 3)
                {
                    bool stepReducedCands = updateCands(u, cell, 'r', vector<int>{cell.col, cell1->col, cell2->col});
                    appendSolvStep(cell.row, cell.col, prefix + "Naked Triplet in row with cell " + cell1->cord2str() + " and cell " + cell2->cord2str(), stepReducedCands);
                    if (stepReducedCands) return true;
                }
            }
            //if cell(row, col) can basically be a naked triple, look for the other part in same col
            cell1 = &at(idx0, cell.col);
            cell2 = &at(idx1, cell.col);
            if (idx0 != idx1 && cell1->row != cell.row && cell2->row != cell.row && cell1->lc() > 1 && cell2->lc() > 1)
            {
                CandSet u = cell.candidates + cell1->candidates + cell2->candidates;
                if (u.size() == 3)
                {
                    bool stepReducedCands = updateCands(u, cell, 'c', vector<int>{cell.row, cell1->row, cell2->row});
                    appendSolvStep(cell.row, cell.col, prefix + "Naked Triplet in col with cell " + cell1->cord2str() + " and cell " + cell2->cord2str(), stepReducedCands);
                    if (stepReducedCands) return true;
                }
            }
            //if cell(row, col) can basically be a naked triple, look for the other part in same block
            cell1 = &atBlock(cell.blk, idx0);
            cell2 = &atBlock(cell.blk, idx1);
            if (idx0 != idx1 && cell1->blkidx != cell.blkidx && cell2->blkidx != cell.blkidx && cell1->lc() > 1 && cell2->lc() > 1)
            {
                CandSet u = cell.candidates + cell1->candidates + cell2->candidates;
                if (u.size() == 3)
                {
                    bool stepReducedCands = updateCands(u, cell, 'b', vector<int>{cell.blkidx, cell1->blkidx, cell2->blkidx});
                    appendSolvStep(cell.row, cell.col, prefix + "Naked Triplet in block with cell " + cell1->cord2str() + " and cell " + cell2->cord2str(), stepReducedCands);
                    if (stepReducedCands) return true;
                }
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForXWing(Cell& cell, string prefix)
{
    if (!cell.isGap())
    {
        return false;
    }
    int row = cell.row;
    int col = cell.col;
    CandSet singleCands, commonC;
    for (int r = row + 1; r < 9; r++)
    {
        for (int c = col + 1; c < 9; c++)
        {
            if (!(at(row, c).isGap() && at(r, col).isGap() && at(r, c).isGap()))
            {
                continue;
            }
            commonC = cell.candidates && at(row, c).candidates && at(r, col).candidates && at(r, c).candidates;
            //check conditions for x-wing column-wise
            singleCands = commonC - collectCands(cell, 'r', vector<int>{col, c}) - collectCands(at(r, col), 'r', vector<int>{col, c});
            bool stepReducedCands = false;
            if (singleCands.size() == 1)
            {
                stepReducedCands |= updateCands(singleCands, at(row, col), 'c', vector<int>{row, r});
                stepReducedCands |= updateCands(singleCands, at(row, c), 'c', vector<int>{row, r});
                appendSolvStep(row, col, prefix + "column-wise X-Wing with diag cell" + at(r, c).cord2str(), stepReducedCands);
                if (stepReducedCands) return true;
            }
            //check conditions for x-wing row-wise
            singleCands = commonC - collectCands(cell, 'c', vector<int>{row, r}) - collectCands(at(row, c), 'c', vector<int>{row, r});
            stepReducedCands = false;
            if (singleCands.size() == 1)
            {
                stepReducedCands |= updateCands(singleCands, at(row, col), 'r', vector<int>{col, c});
                stepReducedCands |= updateCands(singleCands, at(r, col), 'r', vector<int>{col, c});
                appendSolvStep(row, col, prefix + "row-wise X-Wing with diag cell" + at(r, c).cord2str(), stepReducedCands);
                if (stepReducedCands) return true;
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForXYWing(Cell& cell, string prefix)
{
    if (cell.lc() != 2)
    {
        return false;
    }
    int row = cell.row;
    int col = cell.col;
    //check if we have a xy pattern in row-column scenario
    for (int idx0 = 0; idx0 < 9; idx0++)
    {
        for (int idx1 = 0; idx1 < 9; idx1++)
        {
            Cell* xz = &at(row, idx1);
            Cell* yz = &at(idx0, col);
            Cell& ref = at(idx0, idx1);
            if (ref.row != row && ref.col != col && xz->lc() == 2 && yz->lc() == 2 && (cell.candidates + xz->candidates + yz->candidates).size() == 3 && (cell.candidates && xz->candidates && yz->candidates).size() == 0)
            {
                bool stepReducedCands = ref.candidates.remove(xz->candidates && yz->candidates);
                if (stepReducedCands)
                {
                    appendSolvStep(row, col, prefix + "XY wing", stepReducedCands);
                    return true;
                }
            }

            //check if we have a xy pattern in block-row scenario
            xz = &at(row, idx0);
            yz = &atBlock(cell.blk, idx1);
            if (yz->row != row && xz->blk != yz->blk && xz->lc() == 2 && yz->lc() == 2 && (cell.candidates + xz->candidates + yz->candidates).size() == 3 && (cell.candidates && xz->candidates && yz->candidates).size() == 0)
            {
                int r1 = yz->row;
                bool stepReducedCands = false;
                for (int c1 = xz->colBlkPos; c1 < xz->colBlkPos + 3; c1++)
                {
                    stepReducedCands |= at(r1, c1).candidates.remove(xz->candidates && yz->candidates);
                }
                for (int c1 = cell.colBlkPos; c1 < cell.colBlkPos + 3; c1++)
                {
                    stepReducedCands |= at(row, c1).candidates.remove(xz->candidates && yz->candidates);
                }
                if (stepReducedCands)
                {
                    appendSolvStep(row, col, prefix + "XY wing in block-row scenario", stepReducedCands);
                    return true;
                }
            }

            //check if we have a xy pattern in block-column scenario
            xz = &at(idx0, col);
            yz = &atBlock(cell.blk, idx1);
            if (yz->col != col && xz->blk != yz->blk && xz->lc() == 2 && yz->lc() == 2 && (cell.candidates + xz->candidates + yz->candidates).size() == 3 && (cell.candidates && xz->candidates && yz->candidates).size() == 0)
            {
                int c1 = yz->col;
                bool stepReducedCands = false;
                for (int r1 = xz->rowBlkPos; r1 < xz->rowBlkPos + 3; r1++)
                {
                    stepReducedCands |= at(r1, c1).candidates.remove(xz->candidates && yz->candidates);
                }
                for (int r1 = cell.rowBlkPos; r1 < cell.rowBlkPos + 3; r1++)
                {
                    stepReducedCands |= at(r1, col).candidates.remove(xz->candidates && yz->candidates);
                }
                if (stepReducedCands)
                {
                    appendSolvStep(row, col, prefix + "XY wing in block-column scenario", stepReducedCands);
                    return true;
                }
            }
        }
    }
    return false;
}

bool SudokuBoard::checkCellForLockedCandsInBlocks(Cell& cell, string prefix)
{
    if (cell.lc() < 2)
    {
        return false;
    }
    int row = cell.row;
    int col = cell.col;
    int rowstart = cell.rowBlkPos;
    int colstart = cell.colBlkPos;
    CandSet lockedCands;
    //check row-within-block scenario
    vector<int> sameRow = {at(row, colstart).blkidx, at(row, colstart + 1).blkidx, at(row, colstart + 2).blkidx};
    lockedCands = cell.candidates - collectCands(cell, 'b', sameRow);
    if (lockedCands.size() > 0)
    {
        bool stepReducedCands = updateCands(lockedCands, cell, 'r', vector<int>{colstart, colstart + 1, colstart + 2});
        appendSolvStep(row, col, prefix + "Cands [" + lockedCands.to_string() + "] are locked in row", stepReducedCands);
        if (stepReducedCands) return true;
    }
    //check col-within-block scenario
    vector<int> sameCol = {at(rowstart, col).blkidx, at(rowstart + 1, col).blkidx, at(rowstart + 2, col).blkidx};
    lockedCands = cell.candidates - collectCands(cell, 'b', sameCol);
    if (lockedCands.size() > 0)
    {
        bool stepReducedCands = updateCands(lockedCands, cell, 'c', vector<int>{rowstart, rowstart + 1, rowstart + 2});
        appendSolvStep(row, col, prefix + "Cands [" + lockedCands.to_string() + "] are locked in col", stepReducedCands);
        if (stepReducedCands) return true;
    }
    //check block-within-row scenario
    lockedCands = cell.candidates - collectCands(cell, 'r', vector<int>{colstart, colstart + 1, colstart + 2});
    if (lockedCands.size() > 0)
    {
        int exBlkIdx = 3 * (cell.blkidx / 3);
        bool stepReducedCands = updateCands(lockedCands, cell, 'b', vector<int>{exBlkIdx, exBlkIdx + 1, exBlkIdx + 2});
        appendSolvStep(row, col, prefix + "Cands [" + lockedCands.to_string() + "] are locked in block-row", stepReducedCands);
        if (stepReducedCands) return true;
    }
    //check block-within-col scenario
    lockedCands = cell.candidates - collectCands(cell, 'c', vector<int>{rowstart, rowstart + 1, rowstart + 2});
    if (lockedCands.size() > 0)
    {
        int exBlkIdx = cell.blkidx % 3;
        bool stepReducedCands = updateCands(lockedCands, cell, 'b', vector<int>{exBlkIdx, exBlkIdx + 3, exBlkIdx + 6});
        appendSolvStep(row, col, prefix + "Cands [" + lockedCands.to_string() + "] are locked in block-column", stepReducedCands);
        if (stepReducedCands) return true;
    }
    return false;
}

bool SudokuBoard::checkForIntersectingColorPairs(Cell& cell, string prefix, int row1, int col1, int color)
{
    if (cell.lc() != 2)
    {
        return false;
    }
    assert(color == 0 || color == 1);
    int row = cell.row;
    int col = cell.col;
    CandSet candPair = cell.candidates;
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
                //assert(at(r, col1).pairColor == -1);
                //assert(at(row1, c).pairColor == -1);
                bool stepReducedCands = at(r, col1).candidates.remove(candPair) || at(row1, c).candidates.remove(candPair);
                if (stepReducedCands)
                {
                    appendSolvStep(row, col, prefix + "Colored pair: Removed " + candPair.to_string() + " in (" + to_string(r) + ", " + to_string(col1) + ") or (" + to_string(row1) + ", " + to_string(c) + ")", stepReducedCands);
                    return true;
                }
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
                    if (t0.size() != 2 || t0 != t1 || t2.size() != 1) continue;
                    at(row1, col1).candidates = t2;
                    at(r, c).candidates = t2;
                    appendSolvStep(row1, col1, prefix + "Cand pair " + candPair.to_string() + " set due to pair in " + at(i, col1).cord2str() + " and " + at(i, c).cord2str(), true);
                    appendSolvStep(r, c, prefix + "Cand pair " + candPair.to_string() + " set due to pair in " + at(i, col1).cord2str() + " and " + at(i, c).cord2str(), true);
                    return true;
                }
                for (int j = 0; j < 9; j++)
                {
                    if (j == col1 || j == c) continue;
                    CandSet t0 = at(row1, j).candidates;
                    CandSet t1 = at(r, j).candidates;
                    CandSet t2 = candPair - t0;
                    if (t0.size() != 2 || t0 != t1 || t2.size() != 1) continue;
                    at(row1, col1).candidates = t2;
                    at(r, c).candidates = t2;
                    appendSolvStep(row1, col1, prefix + "Cand pair " + candPair.to_string() + " set due to pair in " + at(row1, j).cord2str() + " and " + at(r, j).cord2str(), true);
                    appendSolvStep(r, c, prefix + "Cand pair " + candPair.to_string() + " set due to pair in " + at(row1, j).cord2str() + " and " + at(r, j).cord2str(), true);
                    return true;
                }
            }
        }
    }
    //start the recursive coloring process and build the color tree
    Cell* currCell = &cell;
    if (row1 != -1 && col1 != -1)
    {
        currCell = &at(row1, col1);
    }
    int nextColor = ~color & 1;
    //check if open cells in block are in one line, i.e. in one row or in one column
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
            return checkForIntersectingColorPairs(cell, prefix, nextCell->row, nextCell->col, nextColor);
        }
        nextCell = &at(idx, currCell->col);
        if (nextCell->pairColor == -1 && nextCell->candidates == candPair && nextCell->blk != currCell->blk)
        {
            return checkForIntersectingColorPairs(cell, prefix, nextCell->row, nextCell->col, nextColor);
        }
        nextCell = &atBlock(currCell->blk, idx);
        if (nextCell->pairColor == -1 && nextCell->candidates == candPair)
        {
            return checkForIntersectingColorPairs(cell, prefix, nextCell->row, nextCell->col, nextColor);
        }
        if (numLeftCellsInBlk > 1 && relCol > -1 && !(idx >= currCell->rowBlkPos && idx < currCell->rowBlkPos + 3))
        {
            nextCell = &at(idx, relCol);
            if (nextCell->pairColor == -1 && nextCell->candidates == candPair)
            {
                return checkForIntersectingColorPairs(cell, prefix, nextCell->row, nextCell->col, color);
            }
        }
        if (numLeftCellsInBlk > 1 && relRow > -1 && !(idx >= currCell->colBlkPos && idx < currCell->colBlkPos + 3))
        {
            nextCell = &at(relRow, idx);
            if (nextCell->pairColor == -1 && nextCell->candidates == candPair)
            {
                return checkForIntersectingColorPairs(cell, prefix, nextCell->row, nextCell->col, color);
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
            if (!at(row, col).isGap())
            {
                continue;
            }
            bool tryNext = true;
            if (tryNext) tryNext = !checkCellForNakedSingle(at(row, col), prefix);
            if (tryNext) tryNext = !checkCellForHiddenSingle(at(row, col), prefix);
            if (tryNext) tryNext = !checkCellForNakedPair(at(row, col), prefix);
            if (tryNext) tryNext = !checkCellForHiddenPair(at(row, col), prefix);
            if (tryNext) tryNext = !checkCellForLockedCandsInBlocks(at(row, col), prefix);
            if (tryNext) tryNext = !checkCellForNakedTriplet(at(row, col), prefix);
            if (tryNext) tryNext = !checkCellForXWing(at(row, col), prefix);
            if (tryNext) tryNext = !checkCellForXYWing(at(row, col), prefix);
            if (tryNext) checkForIntersectingColorPairs(at(row, col), prefix);
            if (!isValid())
            {
                appendSolvStep(row, col, "Contradiction occurred!", true);
                return;
            }
        }
    }
}

bool SudokuBoard::tryForcingChain(int numIterations)
{
    array<Cell, 81> origBoard;
    for (int k = 0; k < 81; k++) origBoard[k] = b[k]; //copy
    string origLatexCode = latexCode;
    int it_count;
    for (int r = 0; r < 9; r++)
    {
        for (int c = 0; c < 9; c++)
        {
            if (origBoard[9 * r + c].lc() != 2) continue;
            bool bValid0 = true;
            bool bValid1 = true;
            const int rmCand0 = origBoard[9 * r + c].candidates.getCand(0);
            const int rmCand1 = origBoard[9 * r + c].candidates.getCand(1);
            //use first candidate as solution
            for (int k = 0; k < 81; k++) b[k] = origBoard[k]; //copy
            latexCode = origLatexCode;
            at(r, c).candidates.erase(rmCand1);
            setFinalValue(at(r, c));
            it_count = 0;
            while (bValid0 && (it_count <= numIterations))
            {
                it_count++;
                applyStrategies("Remove cand " + to_string(rmCand1) + " from cell " + at(r, c).cord2str() + ": ");
                bValid0 = isValid();
            }
            bool bSolved0 = bValid0 ? isSolved() : false;
            string latexCode0 = latexCode;
            //use second candidate as solution
            if (!bSolved0 && bValid0) continue;
            for (int k = 0; k < 81; k++) b[k] = origBoard[k]; //copy
            latexCode = origLatexCode;
            at(r, c).candidates.erase(rmCand0);
            setFinalValue(at(r, c));
            it_count = 0;
            while (bValid1 && (it_count <= numIterations))
            {
                it_count++;
                applyStrategies("Remove cand " + to_string(rmCand0) + " from cell " + at(r, c).cord2str() + ": ");
                bValid1 = isValid();
            }
            bool bSolved1 = !bValid0 && bValid1 ? isSolved() : false;
            string latexCode1 = latexCode;
            //check if both quizzes are solvable/invalid or invalid/solvable
            if ((bSolved1 && !bValid0) || (bSolved0 && !bValid1))
            {
                for (int k = 0; k < 81; k++) b[k] = origBoard[k]; //copy
                latexCode = bSolved0 ? latexCode1 : latexCode0;
                at(r, c).candidates.erase(bSolved0 ? rmCand1 : rmCand0);
                setFinalValue(at(r, c));
                appendSolvStep(r, c, "Forcing chain", true);
                while (!isSolved())
                {
                    applyStrategies();
                }
                return true;
            }
        }
    }
    latexCode = origLatexCode;
    return false;
}
    
bool SudokuBoard::solve(int numIterations)
{
    //fill each cand list
    fillCands();
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