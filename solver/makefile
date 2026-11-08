CC=gcc
CFLAGS=-c -std=c++11
LFLAGS=-lstdc++ -lncurses
TARGET=sudoku

OBJDIR=obj
OUTDIR=outputs

SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))

all: $(TARGET)

debug: CFLAGS += -g -Wall
debug: $(TARGET)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR) $(OUTDIR)
	$(CC) $(CFLAGS) $< -o $@

$(TARGET): $(OBJS)
	$(CC) $^ $(LFLAGS) -o $@

$(OBJDIR):
	mkdir -p $@

$(OUTDIR):
	mkdir -p $@

clean:
	rm -r $(OBJDIR)
	rm -r $(OUTDIR)
	rm $(TARGET)
