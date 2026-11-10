#include <set>
#include <array>
#include <vector>
#include <climits>
#include <string>

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
    CandSet(const CandSet& other) { this->data = other.data; }
    /*! Get iterators from std::set.*/
    const std::set<int>::iterator begin() const { return this->data.begin(); }
    const std::set<int>::iterator end() const { return this->data.end(); }
    /*! Gives the number of listed candidates.
        \return dig The number of candidates as an int value. If CandSet::data is empty, 0 is returned.
    */
    int size() { return (int)(this->data.size()); }
    /*! Inserts a specific candidate number into the list.
        \param dig The digit to be inserted into the set (between 1 and 9).
    */
    void insert(int dig) { this->data.insert(dig); }
    /*! Deletes a specific candidate number from list.
        \param dig The digit to be deleted from set (between 1 and 9).
    */
    void erase(int dig) { this->data.erase(dig); }
    /*! Reset the object and delete all candidates in set. After this, CandSet::size is 0.
    */
    void clear() { this->data.clear(); }
    /*! Return candidate at specific position.
        \param pos The position between 0 and size() - 1.
    */
    int getCand(int pos) { return pos == 0 ? *begin() : *next(begin(), pos); }

    bool remove(CandSet set);
    std::string to_string();
    bool operator==(CandSet& op);
    bool operator!=(CandSet& op);
    CandSet operator-(CandSet op);
    CandSet operator&&(CandSet op);
    CandSet operator+(CandSet op);
    CandSet& operator+=(CandSet op);
    CandSet& operator-=(CandSet op);
    CandSet& operator=(const CandSet op);
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
    Cell(): val(0), row(-1), col(-1), blk(-1), blkidx(-1), pairColor(-1) {};
    Cell operator=(const Cell& other);
    /*!Set all class parameters (position and digit) based on the Z-ordered cell index.*/
    void init(int idx, int digit);
    /*!Get current coordinates as formatted string.*/
    std::string cord2str();
    /*! Produces a formatted output string containing the entire set of candidates.*/
    std::string cand2str();
    /*!Is equal to Cell::val?*/
    bool isEq(int dig) { return (val == dig); };
    /*!Is cell unknown?*/
    bool isGap() { return (val == 0); };
    /*!Get length of cands set.*/
    const int lc() { return candidates.size(); };
};

class Board
{
private:
    std::array<Cell, 81> b;
    bool isInCol(int col, int digit);
    bool isInRow(int row, int digit);
    bool isInBlock(int block, int digit);
public:
    Board() {};
    Board(Board& other) { for (int k = 0; k < 81; k++) b[k] = other[k]; }

    const std::array<Cell, 81>::iterator begin() { return b.begin(); }
    const std::array<Cell, 81>::iterator end() { return b.end(); }
    Cell& operator[](int i);
    Board& operator=(Board& other);

    Cell& at(int row, int col);
    Cell& atBlock(int block, int index);
    bool isValid();
    bool isSolved();
    void fillCands();
    std::string getBoardText();
};

class Solver: public Board
{
private:
    //stuff for list of solving steps
    std::vector<std::pair<int, std::string>> solvingSteps;
    std::string latexCode;
    void appendSolvStep(int row, int col, std::string text);
public:
    Solver(): Board() {};
    //output methods
    std::string getStepsText();
    std::string getLatexCode();
    //methods for operating on candidate lists
    CandSet collectCands(Cell& cell, const char mode_r_c_b, std::vector<int> exclude = std::vector<int>{});
    bool updateCands(CandSet digits, Cell& cell, const char mode_r_c_b, std::vector<int> exclude = std::vector<int>{});
    void setFinalValue(Cell& cell);
    //solving techniques
    bool checkCellForNakedSingle(Cell& cell, bool print_step);
    bool checkCellForHiddenSingle(Cell& cell, bool print_step);
    bool checkCellForNakedPair(Cell& cell, bool print_step);
    bool checkCellForHiddenPair(Cell& cell, bool print_step);
    bool checkCellForNakedTriplet(Cell& cell, bool print_step);
    bool checkCellForXWing(Cell& cell, bool print_step);
    bool checkCellForXYWing(Cell& cell, bool print_step);
    bool checkCellForLockedCandsInBlocks(Cell& cell, bool print_step);
    bool checkForIntersectingColorPairs(Cell& cell, bool print_step, int row1 = -1, int col1 = -1, int color = 0);
    bool applyStrategies(Board& quiz, int numIterations, bool print_steps = true);
    bool solve(Board& quiz, int numIterations = INT_MAX);
};