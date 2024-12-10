#include <set>
#include <array>
#include <vector>

/*! CandSet is used to store and manage all candidates of a cell. */
struct CandSet
{
    std::set<int> data;
    /*! Creates an empty candidate container. After creation, the objects internal data variable has size 0.
    */
    CandSet() {};
    /*! Inserts a specific candidate number into the list.
        \param dig The digit to be inserted into the set.
        \return Nothing.
    */
    void insert(int dig) { this->data.insert(dig); }
    /*! Deletes a specific candidate number from list.
        \param dig The digit to be deleted from set.
        \return Nothing.
    */
    void erase(int dig) { this->data.erase(dig); }
    /*! Gives the number of candidates which have been stored.
        \return The size as an int value.
    */
    int size() { return (int)(this->data.size()); }
    void clear() { this->data.clear(); }
    const std::set<int>::iterator begin() const { return this->data.begin(); }
    const std::set<int>::iterator end() const { return this->data.end(); }
    bool operator==(CandSet& op) { return this->data == op.data; }
    bool operator!=(CandSet& op) { return this->data != op.data; }
    bool remove(CandSet& set);
    std::string cand2str();
    /*! Takes the difference of two candidate sets.
        \param op The CandSet that should be subtracted.
        \return The difference as a new CandSet.
    */
    CandSet operator-(CandSet& op);
    /*! Takes the intersection of two candidate sets.
        \param op The second CandSet.
        \return The intersection as a new CandSet.
    */
    CandSet operator&&(CandSet& op);
    /*! Takes the union of two candidate sets.
        \param op The second CandSet.
        \return The union as a new CandSet.
    */
    CandSet operator||(CandSet& op);
    //add + remove candidates, 
    /*! Adds candidates to the current CandSet object.
        \param op The CandSet that should be added.
        \return Reference to the current object.
    */
    CandSet& operator+=(CandSet& op);
    /*! Removes candidates from the current CandSet object.
        \param op The CandSet that should be removed.
        \return Reference to the current object.
    */
    CandSet& operator-=(CandSet& op);
    /*! Copy assignment operator
        \param op Source.
        \return Reference to the current object.
    */
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
    bool solve(int numIterations = INT_MAX);
};