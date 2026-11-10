#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "solver.h"

using namespace std;


void getBoardFromFile(string fname, Board& board)
{
    ifstream in(fname);
    stringstream buffer;
    buffer << in.rdbuf();
    const string sbuf = buffer.str();
    for (int i = 0; i < 81; i++)
    {
        board[i].init(i, atoi(&sbuf.at(2 * i)));
    }
}

void saveLatexCodeToFile(string srcCode, string fname)
{
    ofstream out(fname);
    out << srcCode;
    out.close();
}

int main(int argc, char *argv[])
{
    assert(argc == 2);
    Board board;
    getBoardFromFile(argv[1], board);

    Solver solver;
    bool solved = solver.solve(board, 200);

    cout << solver.getBoardText() << endl;
    cout << solver.getStepsText() << endl;
    saveLatexCodeToFile(solver.getLatexCode(), "outputs/code_steps.tex");

    return (int)solved - 1;
}