#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "solver.h"


void getBoardFromFile(std::string fname, int* board)
{
    std::ifstream in(fname);
    std::stringstream buffer;
    buffer << in.rdbuf();
    const std::string sbuf = buffer.str();
    for (int i = 0; i < 81; i++)
    {
        board[i] = std::atoi(&sbuf.at(2 * i));
    }
}

void saveLatexCodeToFile(std::string& srcCode, std::string fname)
{
    std::ofstream out(fname);
    out << srcCode;
    out.close();
}

int main(int argc, char *argv[])
{
    assert(argc == 2);
    int board[81];
    getBoardFromFile(argv[1], board);

    SudokuBoard sb(board);
    bool solved = sb.solve(200);

    sb.print();
    saveLatexCodeToFile(sb.printSolvingSteps(), "outputs/code_steps.tex");

    return (int)solved - 1;
}