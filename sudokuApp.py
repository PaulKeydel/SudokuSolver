import subprocess
import os
from tkinter import *
from tkinter import filedialog


def writeBoardToFile(board: list, filename: str):
    with open(filename, "w") as outfile:
        for line in range(9):
            outfile.write(" ".join(str(dig) for dig in board[line]))
            outfile.write("\n")
        outfile.close()

def getBoardFromFile(filename: str) -> list:
    board1d = []
    with open(filename, "r") as file:
        board1d = file.read().replace("\n", " ").split(" ")
        file.close()
    board1d = list(map(int, board1d[:81]))
    return [board1d[i:i+9] for i in range(0, 81, 9)]


class SudokuBoard(Tk):
    def __init__(self, screenName = None, baseName = None, className = "Tk", useTk = True, sync = False, use = None):
        super().__init__(screenName, baseName, className, useTk, sync, use)
        self.title("Sudoku Solver")

        self.entries = []
        for row in range(9):
            for col in range(9):
                padl = 10 if col in [0, 3, 6] else 3
                padr = 10 if col in [2, 5, 8] else 3
                padu = 10 if row in [0, 3, 6] else 3
                padd = 10 if row in [2, 5, 8] else 3
                entry = Entry(self, font="Helvetica 46 bold", justify="center", width=2)
                entry.configure(fg="green")
                entry.grid(row=row, column=col, padx=(padl, padr), pady=(padu, padd))
                entry.bind('<KeyPress>', self.handleKeys)
                self.entries.append(entry)
        btn_load = Button(self, text="Lade Testboard", command=self.loadBoard)
        btn_load.grid(row=4, column=9)
        btn_clear = Button(self, text="Lösche Board", command=self.clearBoard)
        btn_clear.grid(row=4, column=10)
        btn_solve = Button(self, text="Löse Sudoku", command=self.solveSudoku)
        btn_solve.grid(row=4, column=11)

    def handleKeys(self, event: Event):
        icurr = 0
        while self.entries[icurr] != self.focus_get():
            icurr += 1
        iprev = icurr - 1 if icurr > 0 else 80
        inext = icurr + 1 if icurr < 80 else 0
        if event.char.isdigit() and len(self.entries[icurr].get()) == 0:
            self.entries[inext].focus_set()
            if event.char == '0':
                return "break"
        elif event.keysym == "BackSpace" and len(self.entries[icurr].get()) == 0:
            self.entries[iprev].delete(0, END)
            self.entries[iprev].focus_set()
        elif event.keysym == "BackSpace" and len(self.entries[icurr].get()) > 0:
            self.entries[icurr].delete(0, END)
        else:
            return "break"

    def loadBoard(self):
        filetypes = (('Textdateien', '*.txt'), ('Alle Dateien', '*.*'))
        filepath = filedialog.askopenfilename(title='Board auswählen', initialdir=os.getcwd() + "/testboards", filetypes=filetypes)
        if not filepath:
            return
        self.clearBoard()
        board = getBoardFromFile(filepath)
        for i in range(81):
            if board[i // 9][i % 9] != 0:
                self.entries[i].insert(0, board[i // 9][i % 9])

    def clearBoard(self):
        for i in range(81):
            self.entries[i].delete(0, END)
            self.entries[i].configure(fg="green")

    def solveSudoku(self):
        board1d = [(int(self.entries[i].get()) if len(self.entries[i].get()) > 0 else 0) for i in range(81)]
        board = [board1d[i:i+9] for i in range(0, 81, 9)]

        writeBoardToFile(board, "last.txt")

        subprocess.call("cd solver && ./sudoku ../last.txt && cd ..", shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)

        # solve Sudoku and fill all gaps in board
        board = getBoardFromFile("solver/outputs/solution.txt")
        for i in range(81):
            if len(self.entries[i].get()) == 0:
                self.entries[i].configure(fg="black")
                self.entries[i].insert(0, board[i // 9][i % 9])

        # LaTeX: generate PDF with solving steps
        subprocess.call("cd latex && pdflatex solving_steps.tex && cd ..", shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)


if __name__ == "__main__":
    app = SudokuBoard()
    app.mainloop()