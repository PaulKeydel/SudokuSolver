import subprocess
import curses


testboard = [
    [0, 0, 0, 2, 0, 0, 0, 1, 7],
    [0, 0, 0, 0, 0, 9, 0, 0, 3],
    [0, 0, 8, 1, 0, 0, 5, 0, 0],
    [6, 0, 0, 9, 0, 0, 0, 5, 0],
    [0, 0, 0, 0, 7, 0, 0, 0, 1],
    [0, 0, 0, 0, 2, 6, 0, 0, 9],
    [0, 0, 0, 8, 0, 0, 0, 2, 6],
    [9, 0, 0, 6, 0, 3, 0, 0, 0],
    [0, 3, 0, 0, 0, 0, 0, 8, 0]
]


def writeBoardToFile(board: list, filename: str):
    with open(filename, "w") as outfile:
        for line in range(9):
            outfile.write(" ".join(str(dig) for dig in board[line]))
            outfile.write("\n")
        outfile.close()

def getBoardFromStdin() -> list:
    board = list()
    stdscr = curses.initscr()
    stdscr.clear()
    stdscr.addstr("Please type in your Sudoku quiz, row by row!\n")
    stdscr.addstr("* use 0 for empty cells\n")
    stdscr.addstr("* type 'c' to correct the last entry\n")
    stdscr.move(4, 0)
    for i in range(9):
        stdscr.addstr("     ┆┆     ┆┆\n")
        if (i == 2 or i == 5):
            stdscr.addstr("===================\n")
    stdscr.move(4, 0)
    stdscr.refresh()
    curses.noecho()
    stdscr.nodelay(False)
    lasty = []
    lastx = []
    while (len(board) < 81):
        row = len(board) // 9
        col = len(board) % 9
        c = stdscr.getch()
        if (c >= 48 and c <= 57):
            # append digit and current pos to the lists
            board.append(c - 48)
            lasty.append(stdscr.getyx()[0])
            lastx.append(stdscr.getyx()[1])
            #print the input
            if (c > 48):
                stdscr.addstr(str(c - 48))
            # move curser to next position
            if (col != 8):
                ypos = 4 + row
                ypos += ((row > 2) + (row > 5))
                xpos = 2 * (col + 1)
                xpos += ((col >= 2) + (col >= 5))
            else:
                xpos = 0
                ypos = 5 + row
                ypos += ((row >= 2) + (row >= 5))
            stdscr.move(ypos, xpos)
        elif ((c == ord('c')) and (len(board) > 0)):
            board.pop()
            ly = lasty.pop()
            lx = lastx.pop()
            stdscr.move(ly, lx)
            stdscr.addstr(" ")
            stdscr.move(ly, lx)
        stdscr.refresh()
    curses.endwin()
    board = [board[i:i + 9] for i in range(0, 81, 9)]
    return board


if __name__ == "__main__":
    #enter quiz and save temporally
    input_board = getBoardFromStdin()
    #input_board = testboard
    writeBoardToFile(input_board, "last.txt")

    #run solver
    print("Solving...")
    subprocess.call("cd solver && ./sudoku ../last.txt && cd ..", shell=True)

    #create PDF file with solving steps
    print("Generating PDF file...")
    subprocess.call("cd latex && pdflatex solving_steps.tex && cd ..", shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)