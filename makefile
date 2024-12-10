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
	$(CC) $(LFLAGS) $^ -o $@

clean:
	rm *.o
	rm $(TARGET)

doc:
	doxygen Doxyfile