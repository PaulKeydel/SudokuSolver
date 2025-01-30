#!/bin/bash

cd solvingSteps
pdflatex solving_steps.tex
open -a "Preview" solving_steps.pdf
cd ..
