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

void saveBoardToFile(Board& board, string fname)
{
    ofstream out(fname);
    out << board.to_text();
    out.close();
}

void saveLatexCodeToFile(string srcCode, string fname)
{
    ofstream out(fname);
    out << srcCode;
    out.close();
}

void printBoardToStdout(Board& board)
{
    for (int row = 0; row < 9; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            cout << to_string(board.at(row, col).val);
            if (col < 8) cout << "  ";
            if (col < 8 && (col % 3 == 2)) cout << "  ";
        }
        cout << endl;
        if (row % 3 == 2) cout << endl;
    }
}


int main(int argc, char *argv[])
{
    assert(argc == 2);
    Board board;
    getBoardFromFile(argv[1], board);

    Solver solver;
    bool solved = solver.solve(board, 200);

    printBoardToStdout((Board&)solver);
    cout << solver.getStepsText() << endl;

    saveBoardToFile((Board&)solver, "outputs/solution.txt");
    saveLatexCodeToFile(solver.getLatexCode(), "outputs/code_steps.tex");

    return (int)solved - 1;
}