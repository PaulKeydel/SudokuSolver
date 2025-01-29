#include <set>
#include <array>
#include <vector>

/*!
    CandSet is used to store and manage all possible candidates of a cell, i.e. the class contains both a container for candidates and several functions to manipulate the set. Manipulating data includes adding new digits, subtracting digits or calculating the union and intersection.

    CandSet::data is an internal container for storing all possible candidates. It's of type std::set<int>.

    To simplify coding, several operators are overloaded in this class. With CandSet it's possible to use =, ==, !=, -, +=, -=, || and &&. The binary || operator calculates the union between two CandSets while the && operator takes the intersection between the left and right operand. The assignment(=) creates a copy of the source.
*/
struct CandSet
{
private:
    std::set<int> data;
public:
    /*! Creates an empty candidate container. After creation, the objects internal data variable has size 0.*/
    CandSet() {};
    /*! Inserts a specific candidate number into the list.
        \param dig The digit to be inserted into the set (between 1 and 9).
    */
    void insert(int dig) { this->data.insert(dig); }
    /*! Deletes a specific candidate number from list.
        \param dig The digit to be deleted from set (between 1 and 9).
    */
    void erase(int dig) { this->data.erase(dig); }
    /*! Gives the number of listed candidates.
        \return dig The number of candidates as an int value. If CandSet::data is empty, 0 is returned.
    */
    int size() { return (int)(this->data.size()); }
    /*! Reset the object and delete all candidates in set. After this, CandSet::size is 0.
    */
    void clear() { this->data.clear(); }
    const std::set<int>::iterator begin() const { return this->data.begin(); }
    const std::set<int>::iterator end() const { return this->data.end(); }
    /*! Removes all candidates that are given by the argument.
        \return <true> if at least one candidate could successfully be removed. <false> if the size could not be reduced.
    */
    bool remove(CandSet& set);
    /*! Produces a formatted output string containing the entire set of candidates.
        \return A string "{cand0, cand1, cand2}".
    */
    std::string cand2str();
    bool operator==(CandSet& op) { return this->data == op.data; }
    bool operator!=(CandSet& op) { return this->data != op.data; }
    CandSet operator-(CandSet& op);
    CandSet operator&&(CandSet& op);
    CandSet operator||(CandSet& op);
    CandSet& operator+=(CandSet& op);
    CandSet& operator-=(CandSet& op);
    CandSet& operator=(const CandSet& op);
};

/*!
    Each of all 81 Sudoku cells are mapped to an object of the class Cell. The Cell class contains the coordinates, the digit (0 if it's a gap) and the color parameter for the coloring pair algorithm. Here is an overview about the class members:
    
    Cell::val is an int and contains the digit (0 <= val <=9, 0 = empty).

    Cell::row and Cell::col are used to index all board cells. The (row, col)-coordinate system starts in the upper left with (row, col) = (0, 0) and ends with (8, 8) in the bottom right.

    Cell::blk and Cell::blkidx form a coordinate system based on the 3x3-subblocks which will be counted in Z-scan order, starting with subblock blk = 0 in the upper left. blkidx addresses all 9 cells within the 3x3 block by row-wise indexing, 0 <= blkidx < 9.

    Cell::rowBlkPos and Cell::colBlkPos are (row, col)-coordinates referencing to the upper left cell within the current subblock. They are generally multiple of 3 (= 0, 3 or 6).

    Cell::candidates is used to store and manage all possible candidates. It's an object of class CandSet. If Cell::val is between 1 and 9, Cell::candidates will be an empty set.

    Cell::pairColor is needed for pairing colors. All identical candidate pairs in board can be colorized by setting Cell::pairColor alternately to 0 and 1.
*/
struct Cell
{
    //digit of the cell
    int val;
    //position parameters
    int row;
    int col;
    int blk;
    int blkidx;
    //upper left cell in current subblock
    int rowBlkPos;
    int colBlkPos;
    //color for color pair algorithm
    int pairColor;
    //set for storing candidates
    CandSet candidates;
    //methods
    Cell() {};
    /*!Set all class parameters (position and digit) based on the Z-ordered cell index.*/
    void init(int idx, int digit);
    /*!Get current coordinates as formatted string.*/
    std::string cord2str();
    /*!Is equal to Cell::val?*/
    bool isEq(int dig) { return (this->val == dig); };
    /*!Is cell unknown?*/
    bool isGap() { return (this->val == 0); };
    /*!Get length of cands set.*/
    const int lc() { return this->candidates.size(); };
};

/*!
    SudokuBoard represents the whole board and comprises all 81 cells of type Cell. The class additionally includes methods for collecting and updating candidates as well as solving techniques.

    Use SudokuBoard::solve to find the solution of the current quiz. The algorithm iteratively applies all implemented techniques to each cell. The methods SudokuBoard::print and SudokuBoard::printSolvingSteps print the resulting board and all effective solving steps.

    The first step of solving is to collect all candidates in empty cells. For this, SudokuBoard::solve calls SudokuBoard::collectCands. For each empty cell SudokuBoard::collectCands checks if a digit between 1 and 9 is missing in current row, column and block by calling helper functions SudokuBoard::isInRow, SudokuBoard::isInCol, SudokuBoard::isInBlock.

    The next step is the strategic solving. Here the approach of the implemented techniques depends on whether we have only one or more candidates in the list. If there is a naked single or a hidden single, we'll obtain a candidate set with only one entry. Thus, Cell::val can directly be set and the found digit can be removed from all candidate sets in same row, column and subblock. The process of setting Cell::val and updating the corresponding row, column or block is condensed in SudokuBoard::setFinalValue.
    All other solving techniques are used to piecewise reduce the CandSet by considering logic patterns and dependencies. Elimination either covers the own CandSet or the sets in neighborhood. In order to check if an elimination was successful in a row, column or block sub-structure, the algorithm utilises SudokuBoard::updateCandsInRow, SudokuBoard::updateCandsInRow and SudokuBoard::updateCandsInRow. Only if all existing candidate sets can be reduced to size 1 the Sudoku solver can give a complete solution.
*/
class SudokuBoard
{
private:
    std::array<Cell, 81> b;
    //stuff for list of solving steps
    std::vector<std::pair<int, std::string>> solvingSteps;
    std::string latexCode;
    void appendSolvStep(int row, int col, std::string text, bool bReducedCands);
public:
    SudokuBoard(int* board);
    void print();
    std::string& printSolvingSteps();
    Cell& at(int row, int col);
    Cell& atBlock(int block, int index);
    bool isInCol(int col, int digit);
    bool isInRow(int row, int digit);
    bool isInBlock(int block, int digit);
    bool valid();
    //methods for managing candidate list
    void collectCands();
    bool updateCandsInRow(int row, std::vector<int> excludedPositions, CandSet digits);
    bool updateCandsInCol(int col, std::vector<int> excludedPositions, CandSet digits);
    bool updateCandsInBlock(int blk, std::vector<int> excludedPositions, CandSet digits);
    void setFinalValue(int row, int col);
    //solving techniques
    bool checkCellForNakedSingle(int row, int col);
    bool checkCellForHiddenSingle(int row, int col);
    bool checkCellForNakedPair(int row, int col);
    bool checkCellForHiddenPair(int row, int col);
    bool checkCellForNakedTriplet(int row, int col);
    bool checkCellForXWing(int row, int col);
    bool checkCellForXYWing(int row, int col);
    bool checkCellForLockedCandsInBlocks(int row, int col);
    bool checkForIntersectingColorPairs(int row, int col, int row1 = -1, int col1 = -1, int color = 0);
    void applyStrategies();
    bool solve(int numIterations = INT_MAX);
};