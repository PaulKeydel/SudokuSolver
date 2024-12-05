#include <set>
#include <array>

struct CandSet
{
    std::set<int> data;
    CandSet() {};
    void insert(int dig) { this->data.insert(dig); }
    void erase(int dig) { this->data.erase(dig); }
    int size() { return (int)(this->data.size()); }
    void clear() { this->data.clear(); }
    const std::set<int>::iterator begin() const { return this->data.begin(); }
    const std::set<int>::iterator end() const { return this->data.end(); }
    bool operator==(CandSet& op) { return this->data == op.data; }
    bool operator!=(CandSet& op) { return this->data != op.data; }
    int at(int index);
    bool remove(CandSet& set);
    std::string cand2str();
    //take the difference
    CandSet operator-(CandSet& op);
    //take the intersection
    CandSet operator&&(CandSet& op);
    //take the union
    CandSet operator||(CandSet& op);
    //add + remove candidates, copy assignment
    CandSet& operator+=(CandSet& op);
    CandSet& operator-=(CandSet& op);
    CandSet& operator=(const CandSet& op);
};

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
    void init(int idx, int digit);
    std::string cord2str();
    bool isEq(int dig) { return (this->val == dig); };
    bool isGap() { return (this->val == 0); };
    //length of cands set
    const int lc() { return (int)this->candidates.data.size(); };
};

class SudokuBoard
{
private:
    std::array<Cell, 81> b;
    //stuff for list of solving steps
    std::vector<std::pair<int, std::string>> solvingSteps;
    void appendSolvStep(int row, int col, std::string text, bool bReducedCands);
public:
    SudokuBoard(int* board);
    void print();
    void printSolvingSteps();
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
    void checkForIntersectingColorPairs(int row, int col, int row1 = -1, int col1 = -1, int color = 0);
    void applyStrategies();
    void solve(int numIterations = INT_MAX);
};