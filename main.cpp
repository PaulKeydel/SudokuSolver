#include <curses.h>
#include <vector>
//#include <stdlib.h>
#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "solver.h"


void getBoardFromStdin(int* board)
{
    WINDOW* stdscr = initscr();
    clear();
    addstr("Please type in your Sudoku quiz, row by row!\n");
    addstr("* use 0 for empty cells\n");
    addstr("* type 'c' to correct the last entry\n");
    move(4, 0);
    for (int i = 0; i < 9; i++)
    {
        addstr("     ||     ||\n");
        if (i == 2 || i == 5)
        {
            addstr("===================\n");
        }
    }
    move(4, 0);
    refresh();
    noecho();
    nodelay(stdscr, false);
    std::vector<int> lasty;
    std::vector<int> lastx;
    int num_added = 0;
    while (num_added < 81)
    {
        int row = num_added / 9;
        int col = num_added % 9;
        char c = getchar();
        if (c >= 48 && c <= 57)
        {
            //append digit and current pos to the lists
            *(board + num_added) = (c - 48);
            num_added++;
            lasty.push_back(getcury(stdscr));
            lastx.push_back(getcurx(stdscr));
            //print the input
            if (c > 48)
            {
                addstr(std::to_string(c - 48).c_str());
            }
            //move curser to next position
            int xpos, ypos;
            if (col != 8)
            {
                ypos = 4 + row;
                ypos += ((row > 2) + (row > 5));
                xpos = 2 * (col + 1);
                xpos += ((col >= 2) + (col >= 5));
            }
            else
            {
                xpos = 0;
                ypos = 5 + row;
                ypos += ((row >= 2) + (row >= 5));
            }
            move(ypos, xpos);
        }
        else if ((c == 'c') && (num_added > 0))
        {
            num_added--;
            int ly = lasty.back();
            int lx = lastx.back();
            lasty.pop_back();
            lastx.pop_back();
            move(ly, lx);
            addstr(" ");
            move(ly, lx);
        }
        refresh();
    }
    endwin();
}

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

void saveBoardToFile(std::string fname, int* board)
{
    std::string sbuf = "";
    for (int i = 0; i < 81; i++)
    {
        sbuf += std::to_string(board[i]);
        sbuf += (i % 9 == 8) ? "\n" : " ";
    }
    std::ofstream out(fname);
    out << sbuf;
    out.close();
}

int main(int argc, char *argv[])
{
    int board[81];
    if (argc == 2)
    {
        getBoardFromFile(argv[1], board);
    }
    else
    {
        getBoardFromStdin(board);
        saveBoardToFile("last.txt", board);
    }
    SudokuBoard sb(board);
    bool solved = sb.solve(1000);
    sb.print();
    sb.printSolvingSteps();
    std::cout << (solved ? "Solved" : "Not solvable") << std::endl;
    return (int)solved - 1;
}