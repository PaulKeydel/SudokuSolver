#include <curses.h>
#include <vector>
#include <stdlib.h>
#include <cassert>
#include <iostream>
#include "solver.h"

int TestBoard1[81] =
{
    8, 4, 0, 6, 5, 0, 0, 2, 0,
    0, 0, 3, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 7, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 6, 0,
    0, 0, 0, 0, 6, 0, 0, 0, 8,
    9, 0, 6, 0, 0, 4, 0, 0, 1,
    0, 5, 0, 3, 7, 0, 0, 8, 0,
    0, 0, 0, 8, 0, 0, 3, 0, 7,
    0, 0, 0, 0, 0, 9, 4, 0, 0
};

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

int main()
{
    int board[81];
#if 1
    memcpy(board, TestBoard1, 81 * sizeof(int));
#else
    getBoardFromStdin(board);
#endif
    SudokuBoard sb(board);
    sb.solve(100);
    sb.print();
    sb.printSolvingSteps();
    return 0;
}