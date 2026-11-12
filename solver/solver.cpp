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


Cell& Board::operator[](int i)
{
    return b[i];
}

Board& Board::operator=(Board& other)
{
    for (int k = 0; k < 81; k++) b[k] = other[k];
    return *this;
}

Cell& Board::at(int row, int col)
{
    assert(col >= 0 && col < 9);
    assert(row >= 0 && row < 9);
    return b.at(9 * row + col);
}

Cell& Board::atBlock(int block, int index)
{
    assert(block >= 0 && block < 9);
    assert(index >= 0 && index < 9);
    int row_ = 3 * (block / 3) + (index / 3);
    int col_ = 3 * (block % 3) + (index % 3);
    return b.at(9 * row_ + col_);
}

bool Board::isInCol(int col, int digit)
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

bool Board::isInRow(int row, int digit)
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

bool Board::isInBlock(int block, int digit)
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

bool Board::isValid()
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

bool Board::isSolved()
{
    for (Cell& cell : b)
    {
        if (cell.isGap()) return false;
    }
    return isValid();
}

void Board::fillCands()
{
    for (Cell& cell : *this)
    {
        cell.candidates.clear();
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

string Board::to_text()
{
    string boardText = "";
    for (int row = 0; row < 9; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            boardText += to_string(this->at(row, col).val);
            if (col < 8) boardText += " ";
        }
        boardText += "\n";
    }
    return boardText;
}


void Solver::appendSolvStep(int row, int col, string text)
{
    pair<int, string> p = make_pair(9 * row + col, text);
    solvingSteps.push_back(p);
    //add latex code
    latexCode += "\\section{" + text + "}\n" + "\\sudoku{\n";
    for (Cell& cell : *this)
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

string Solver::getStepsText()
{
    string stepsText = "";
    for (pair<int, string>& step : solvingSteps)
    {
        int row = get<0>(step) / 9;
        int col = get<0>(step) % 9;
        string text = get<1>(step);
        stepsText += "(" + to_string(row) + ", " + to_string(col) + "): " + text + "\n";
    }
    return stepsText;
}

string Solver::getLatexCode()
{
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
    return latexCode;
}

CandSet Solver::collectCands(Cell& cell, const char mode_r_c_b, vector<int> exclude)
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

bool Solver::updateCands(CandSet digits, Cell& cell, const char mode_r_c_b, vector<int> exclude)
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

void Solver::setFinalValue(Cell& cell)
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

bool Solver::checkCellForNakedSingle(Cell& cell, bool print_step)
{
    if (cell.lc() != 1)
    {
        return false;
    }
    setFinalValue(cell);
    if (print_step) appendSolvStep(cell.row, cell.col, "Naked Single");
    return true;
}

bool Solver::checkCellForHiddenSingle(Cell& cell, bool print_step)
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
        if (print_step) appendSolvStep(cell.row, cell.col, "Hidden Single");
        return true;
    }
    //search along col and collect all other candidates
    uniqueC = cell.candidates - collectCands(cell, 'c', vector<int>{cell.row});
    if (uniqueC.size() == 1)
    {
        cell.candidates = uniqueC;
        setFinalValue(cell);
        if (print_step) appendSolvStep(cell.row, cell.col, "Hidden Single");
        return true;
    }
    //search within block and collect all other candidates
    uniqueC = cell.candidates - collectCands(cell, 'b', vector<int>{cell.blkidx});
    if (uniqueC.size() == 1)
    {
        cell.candidates = uniqueC;
        setFinalValue(cell);
        if (print_step) appendSolvStep(cell.row, cell.col, "Hidden Single");
        return true;
    }
    return false;
}

bool Solver::checkCellForNakedPair(Cell& cell, bool print_step)
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
            if (updateCands(candPair, cell, 'r', vector<int>{cell.col, cell1->col}))
            {
                if (print_step) appendSolvStep(cell.row, cell.col, "Naked Pair with cell " + cell1->cord2str());
                return true;
            }
        }
        //if cell(row, col) can basically be a naked pair, look for the other part in same col
        cell1 = &at(idx, cell.col);
        if (cell1->row != cell.row && cell1->candidates == candPair)
        {
            if (updateCands(candPair, cell, 'c', vector<int>{cell.row, cell1->row}))
            {
                if (print_step) appendSolvStep(cell.row, cell.col, "Naked Pair with cell " + cell1->cord2str());
                return true;
            }
        }
        //if cell(row, col) can basically be a naked pair, look for the other part in same block
        cell1 = &atBlock(cell.blk, idx);
        if (cell1->blkidx != cell.blkidx && cell1->candidates == candPair)
        {
            if (updateCands(candPair, cell, 'b', vector<int>{cell.blkidx, cell1->blkidx}))
            {
                if (print_step) appendSolvStep(cell.row, cell.col, "Naked Pair with cell " + cell1->cord2str());
                return true;
            }
        }
    }
    return false;
}

bool Solver::checkCellForHiddenPair(Cell& cell, bool print_step)
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
                if (print_step) appendSolvStep(cell.row, cell.col, "Hidden Pair " + cell.cand2str() + " with cell " + cell1->cord2str());
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
                if (print_step) appendSolvStep(cell.row, cell.col, "Hidden Pair " + cell.cand2str() + " with cell " + cell1->cord2str());
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
                if (print_step) appendSolvStep(cell.row, cell.col, "Hidden Pair " + cell.cand2str() + " with cell " + cell1->cord2str());
                return true;
            }
        }
    }
    return false;
}

bool Solver::checkCellForNakedTriplet(Cell& cell, bool print_step)
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
                if (u.size() == 3 && updateCands(u, cell, 'r', vector<int>{cell.col, cell1->col, cell2->col}))
                {
                    if (print_step) appendSolvStep(cell.row, cell.col, "Naked Triplet in row with cell " + cell1->cord2str() + " and cell " + cell2->cord2str());
                    return true;
                }
            }
            //if cell(row, col) can basically be a naked triple, look for the other part in same col
            cell1 = &at(idx0, cell.col);
            cell2 = &at(idx1, cell.col);
            if (idx0 != idx1 && cell1->row != cell.row && cell2->row != cell.row && cell1->lc() > 1 && cell2->lc() > 1)
            {
                CandSet u = cell.candidates + cell1->candidates + cell2->candidates;
                if (u.size() == 3 && updateCands(u, cell, 'c', vector<int>{cell.row, cell1->row, cell2->row}))
                {
                    if (print_step) appendSolvStep(cell.row, cell.col, "Naked Triplet in col with cell " + cell1->cord2str() + " and cell " + cell2->cord2str());
                    return true;
                }
            }
            //if cell(row, col) can basically be a naked triple, look for the other part in same block
            cell1 = &atBlock(cell.blk, idx0);
            cell2 = &atBlock(cell.blk, idx1);
            if (idx0 != idx1 && cell1->blkidx != cell.blkidx && cell2->blkidx != cell.blkidx && cell1->lc() > 1 && cell2->lc() > 1)
            {
                CandSet u = cell.candidates + cell1->candidates + cell2->candidates;
                if (u.size() == 3 && updateCands(u, cell, 'b', vector<int>{cell.blkidx, cell1->blkidx, cell2->blkidx}))
                {
                    if (print_step) appendSolvStep(cell.row, cell.col, "Naked Triplet in block with cell " + cell1->cord2str() + " and cell " + cell2->cord2str());
                    return true;
                }
            }
        }
    }
    return false;
}

bool Solver::checkCellForXWing(Cell& cell, bool print_step)
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
            if (singleCands.size() == 1)
            {
                if (updateCands(singleCands, cell, 'c', vector<int>{row, r}) || updateCands(singleCands, at(row, c), 'c', vector<int>{row, r}))
                {
                    if (print_step) appendSolvStep(row, col, "column-wise X-Wing with diag cell" + at(r, c).cord2str());
                    return true;
                }
            }
            //check conditions for x-wing row-wise
            singleCands = commonC - collectCands(cell, 'c', vector<int>{row, r}) - collectCands(at(row, c), 'c', vector<int>{row, r});
            if (singleCands.size() == 1)
            {
                if (updateCands(singleCands, cell, 'r', vector<int>{col, c}) || updateCands(singleCands, at(r, col), 'r', vector<int>{col, c}))
                {
                    if (print_step) appendSolvStep(row, col, "row-wise X-Wing with diag cell" + at(r, c).cord2str());
                    return true;
                }
            }
        }
    }
    return false;
}

bool Solver::checkCellForXYWing(Cell& cell, bool print_step)
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
                if (ref.candidates.remove(xz->candidates && yz->candidates))
                {
                    if (print_step) appendSolvStep(row, col, "XY wing");
                    return true;
                }
            }
            //check if we have a xy pattern in block-row scenario
            xz = &at(row, idx0);
            yz = &atBlock(cell.blk, idx1);
            if (yz->row != row && xz->blk != yz->blk && xz->lc() == 2 && yz->lc() == 2 && (cell.candidates + xz->candidates + yz->candidates).size() == 3 && (cell.candidates && xz->candidates && yz->candidates).size() == 0)
            {
                int r1 = yz->row;
                CandSet rem = xz->candidates && yz->candidates;
                int c1 = xz->colBlkPos;
                bool reducedCands = at(r1, c1).candidates.remove(rem) || at(r1, c1 + 1).candidates.remove(rem) || at(r1, c1 + 2).candidates.remove(rem);
                c1 = cell.colBlkPos;
                reducedCands |= at(row, c1).candidates.remove(rem) || at(row, c1 + 1).candidates.remove(rem) || at(row, c1 + 2).candidates.remove(rem);
                if (reducedCands)
                {
                    if (print_step) appendSolvStep(row, col, "XY wing in block-row scenario");
                    return true;
                }
            }
            //check if we have a xy pattern in block-column scenario
            xz = &at(idx0, col);
            yz = &atBlock(cell.blk, idx1);
            if (yz->col != col && xz->blk != yz->blk && xz->lc() == 2 && yz->lc() == 2 && (cell.candidates + xz->candidates + yz->candidates).size() == 3 && (cell.candidates && xz->candidates && yz->candidates).size() == 0)
            {
                int c1 = yz->col;
                CandSet rem = xz->candidates && yz->candidates;
                int r1 = xz->rowBlkPos;
                bool reducedCands = at(r1, c1).candidates.remove(rem) || at(r1 + 1, c1).candidates.remove(rem) || at(r1 + 2, c1).candidates.remove(rem);
                r1 = cell.rowBlkPos;
                reducedCands |= at(r1, col).candidates.remove(rem) || at(r1 + 1, col).candidates.remove(rem) || at(r1 + 2, col).candidates.remove(rem);
                if (reducedCands)
                {
                    if (print_step) appendSolvStep(row, col, "XY wing in block-column scenario");
                    return true;
                }
            }
        }
    }
    return false;
}

bool Solver::checkCellForLockedCandsInBlocks(Cell& cell, bool print_step)
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
    if (lockedCands.size() > 0 && updateCands(lockedCands, cell, 'r', vector<int>{colstart, colstart + 1, colstart + 2}))
    {
        if (print_step) appendSolvStep(row, col, "Cands [" + lockedCands.to_string() + "] are locked in row");
        return true;
    }
    //check col-within-block scenario
    vector<int> sameCol = {at(rowstart, col).blkidx, at(rowstart + 1, col).blkidx, at(rowstart + 2, col).blkidx};
    lockedCands = cell.candidates - collectCands(cell, 'b', sameCol);
    if (lockedCands.size() > 0 && updateCands(lockedCands, cell, 'c', vector<int>{rowstart, rowstart + 1, rowstart + 2}))
    {
        if (print_step) appendSolvStep(row, col, "Cands [" + lockedCands.to_string() + "] are locked in col");
        return true;
    }
    //check block-within-row scenario
    lockedCands = cell.candidates - collectCands(cell, 'r', vector<int>{colstart, colstart + 1, colstart + 2});
    vector<int> exclBlkRow = {at(cell.row, cell.colBlkPos).blkidx, at(cell.row, cell.colBlkPos + 1).blkidx, at(cell.row, cell.colBlkPos + 2).blkidx};
    if (lockedCands.size() > 0 && updateCands(lockedCands, cell, 'b', exclBlkRow))
    {
        if (print_step) appendSolvStep(row, col, "Cands [" + lockedCands.to_string() + "] are locked in block-row");
        return true;
    }
    //check block-within-colunm scenario
    lockedCands = cell.candidates - collectCands(cell, 'c', vector<int>{rowstart, rowstart + 1, rowstart + 2});
    vector<int> exclBlkCol = {at(cell.rowBlkPos, cell.col).blkidx, at(cell.rowBlkPos + 1, cell.col).blkidx, at(cell.rowBlkPos + 2, cell.col).blkidx};
    if (lockedCands.size() > 0 && updateCands(lockedCands, cell, 'b', exclBlkCol))
    {
        if (print_step) appendSolvStep(row, col, "Cands [" + lockedCands.to_string() + "] are locked in block-column");
        return true;
    }
    return false;
}

bool Solver::checkForIntersectingColorPairs(Cell& cell, bool print_step, Cell* curr, int color)
{
    if (cell.lc() != 2)
    {
        return false;
    }
    assert(color == 0 || color == 1);
    //initialize/reset color at start
    if (curr == nullptr)
    {
        for (Cell& c_ : *this) c_.pairColor = -1;
        curr = &cell;
    }
    //start the recursive coloring process and build the color tree
    assert(curr->lc() == 2);
    curr->pairColor  = color;
    int nextColor    = ~color & 1;
    CandSet candPair = cell.candidates;
    //check if open cells in block are in one line, i.e. in one row or in one column
    int numLeftCellsInBlk = 0;
    int relRow = -1;
    int relCol = -1;
    for (int bi = 0; bi < 9; bi++)
    {
        if (bi == curr->blkidx) continue;
        if (atBlock(curr->blk, bi).isGap())
        {
            numLeftCellsInBlk++;
            if (numLeftCellsInBlk == 1)
            {
                relRow = atBlock(curr->blk, bi).row;
                relCol = atBlock(curr->blk, bi).col;
            }
            if (numLeftCellsInBlk > 1 && relRow != atBlock(curr->blk, bi).row) relRow = -1;
            if (numLeftCellsInBlk > 1 && relCol != atBlock(curr->blk, bi).col) relCol = -1;
        }
    }
    for (Cell& nextC : *this)
    {
        //update cand list if we have an intersecting color pair
        if (nextC.row != curr->row && nextC.col != curr->col && nextC.pairColor == nextColor)
        {
            if (at(nextC.row, curr->col).candidates.remove(candPair) || at(curr->row, nextC.col).candidates.remove(candPair))
            {
                if (print_step) appendSolvStep(cell.row, cell.col, "Colored pair: Removed " + candPair.to_string() + " in (" + to_string(nextC.row) + ", " + to_string(curr->col) + ") or (" + to_string(curr->row) + ", " + to_string(nextC.col) + ")");
                return true;
            }
        }
        //determine candPair by a Naked Pair that has only one candidate in common
        if (nextC.row != curr->row && nextC.col != curr->col && nextC.pairColor == color)
        {
            for (int i = 0; i < 9; i++)
            {
                if (i == curr->row || i == nextC.row) continue;
                CandSet t0 = at(i, curr->col).candidates;
                CandSet t1 = at(i, nextC.col).candidates;
                if (t0.size() == 2 && t1.size() == 2 && t0 == t1 && (t0 && candPair).size() == 1)
                {
                    curr->candidates.remove(t0 && candPair);
                    nextC.candidates.remove(t0 && candPair);
                    if (print_step) appendSolvStep(curr->row, curr->col, "Colored pair " + candPair.to_string() + " could be reduced due to a constellation with another Naked pair sharing one common candidate");
                    return true;
                }
            }
            for (int j = 0; j < 9; j++)
            {
                if (j == curr->col || j == nextC.col) continue;
                CandSet t0 = at(curr->row, j).candidates;
                CandSet t1 = at(nextC.row, j).candidates;
                if (t0.size() == 2 && t1.size() == 2 && t0 == t1 && (t0 && candPair).size() == 1)
                {
                    curr->candidates.remove(t0 && candPair);
                    nextC.candidates.remove(t0 && candPair);
                    if (print_step) appendSolvStep(curr->row, curr->col, "Colored pair " + candPair.to_string() + " could be reduced due to a constellation with another Naked pair sharing one common candidate");
                    return true;
                }
            }
        }
        //recursive calls
        if (nextC.pairColor != -1 || nextC.candidates != candPair)
        {
            continue;
        }
        if (nextC.row == curr->row && nextC.blk != curr->blk)
        {
            return checkForIntersectingColorPairs(cell, print_step, &nextC, nextColor);
        }
        if (nextC.col == curr->col && nextC.blk != curr->blk)
        {
            return checkForIntersectingColorPairs(cell, print_step, &nextC, nextColor);
        }
        if (nextC.blk == curr->blk)
        {
            return checkForIntersectingColorPairs(cell, print_step, &nextC, nextColor);
        }
        if (numLeftCellsInBlk > 0 && relCol > -1 && nextC.col == relCol && nextC.blk != curr->blk)
        {
            return checkForIntersectingColorPairs(cell, print_step, &nextC, color);
        }
        if (numLeftCellsInBlk > 0 && relRow > -1 && nextC.row == relRow && nextC.blk != curr->blk)
        {
            return checkForIntersectingColorPairs(cell, print_step, &nextC, color);
        }
    }
    return false;
}

bool Solver::applyStrategies(Board& quiz, int numIterations, bool print_steps)
{
    //copy from given board
    (Board&)*this = quiz;
    //fill each cell with its candidates
    fillCands();
    //try to solve
    int it_count = 0;
    bool solved = false;
    while (!solved && (it_count <= numIterations))
    {
        it_count++;
        for (Cell& cell : *this)
        {
            if (!cell.isGap())
            {
                continue;
            }
            bool tryNext = true;
            if (tryNext) tryNext = !checkCellForNakedSingle(cell, print_steps);
            if (tryNext) tryNext = !checkCellForHiddenSingle(cell, print_steps);
            if (tryNext) tryNext = !checkCellForNakedPair(cell, print_steps);
            if (tryNext) tryNext = !checkCellForHiddenPair(cell, print_steps);
            if (tryNext) tryNext = !checkCellForLockedCandsInBlocks(cell, print_steps);
            if (tryNext) tryNext = !checkCellForNakedTriplet(cell, print_steps);
            if (tryNext) tryNext = !checkCellForXWing(cell, print_steps);
            if (tryNext) tryNext = !checkCellForXYWing(cell, print_steps);
            if (tryNext) checkForIntersectingColorPairs(cell, print_steps);
            if (!isValid())
            {
                return false;
            }
        }
        solved = isSolved();
    }
    return solved;
}
    
bool Solver::solve(Board& quiz, int numIterations)
{
    //reset Latex code and try to solve
    latexCode = "";
    bool bIsSolved = applyStrategies(quiz, numIterations, true);
    //forcing chain
    if (!bIsSolved)
    {
        Board b1 = (Board&)*this;
        Board b2 = (Board&)*this;
        for (Cell& cell : *this)
        {
            if (cell.lc() != 2)
            {
                continue;
            }
            const int rmCand1 = cell.candidates.getCand(0);
            const int rmCand2 = cell.candidates.getCand(1);
            b1.at(cell.row, cell.col).val = rmCand1;
            b2.at(cell.row, cell.col).val = rmCand2;
            //check both options
            bool bSolved1 = applyStrategies(b1, numIterations, false);
            bool bValid1 = isValid();
            bool bSolved2 = applyStrategies(b2, numIterations, false);
            bool bValid2 = isValid();
            //successful forcing chain?
            if (bSolved1 && !bValid2)
            {
                appendSolvStep(cell.row, cell.col, "Forcing chain: Could remove cand " + to_string(rmCand2) + " in cell " + cell.cord2str());
                return applyStrategies(b1, numIterations); //is going to return true
            }
            else if (bSolved2 && !bValid1)
            {
                appendSolvStep(cell.row, cell.col, "Forcing chain: Could remove cand " + to_string(rmCand1) + " in cell " + cell.cord2str());
                return applyStrategies(b2, numIterations); //is going to return true
            }
            else
            {
                //restore old state
                b1.at(cell.row, cell.col).val = 0;
                b2.at(cell.row, cell.col).val = 0;
            }
        }
    }
    return bIsSolved;
}