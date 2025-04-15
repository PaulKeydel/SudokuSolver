CC=gcc
CFLAGS=-c -std=c++11
LFLAGS=-lstdc++ -lncurses
TARGET=sudoku

SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

all: $(TARGET)

debug: CFLAGS += -g -Wall
debug: $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

$(TARGET): $(OBJS)
	$(CC) $^ $(LFLAGS) -o $@

clean:
	rm *.o
	rm $(TARGET)

.PHONY: doc
doc:
	cd doc; doxygen Doxyfile
	cd doc/latex; make
	cp doc/latex/refman.pdf doc/refman.pdf
