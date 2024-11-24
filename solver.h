#include <set>
#include <array>

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
    std::set<int> candidates;
    //methods
    Cell() {};
    Cell(int idx, int digit);
    Cell& operator=(Cell&& T) = default;
    std::string getOutput();
    bool isEq(int dig) { return (this->val == dig); };
    bool isGap() { return (this->val == 0); };
    //length of cands set and contain-function
    const int lc() { return (int)this->candidates.size(); };
    const bool hasC(int dig) { return (candidates.find(dig) != candidates.end()); };
};

class SudokuBoard
{
private:
    std::array<Cell, 81> b;
    //take the difference
    friend std::set<int> operator-(std::set<int>& op1, std::set<int>& op2);
    //take the intersection
    friend std::set<int> operator&&(std::set<int>& op1, std::set<int>& op2);
    //take the union
    friend std::set<int> operator||(std::set<int>& op1, std::set<int>& op2);
    //stuff for list of solving steps
    std::vector<std::pair<int, std::string>> solvingSteps;
    void appendSolvStep(int row, int col, std::string text, bool bReducedCands);
    std::string cand2str(std::set<int> cands);
    std::string cord2str(int row, int col);
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
    bool updateCandsInRow(int row, std::vector<int> excludedPositions, int dig);
    bool updateCandsInCol(int col, std::vector<int> excludedPositions, int dig);
    bool updateCandsInBlock(int blk, std::vector<int> excludedPositions, int dig);
    void updateCandsFromSolvedCell(int row, int col);
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