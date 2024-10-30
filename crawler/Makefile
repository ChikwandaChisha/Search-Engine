# Makefile for crawler program in CS50 TSE
# Chikwanda Chisha
# October 25, 2024

# Compiler
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common
LIBS = ../libcs50/libcs50.a ../common/common.a

# Executable
EXE = crawler

# Files
OBJS = crawler.o
TEST_SCRIPT = testing.sh

# Default target
$(EXE): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(EXE)

# Compile .c files to .o files
crawler.o: crawler.c ../common/pagedir.h

# Clean up
clean:
	rm -f $(OBJS) $(EXE) *.out
	rm -rf ../data/*

# Test target
test: $(EXE)
	./$(TEST_SCRIPT)

# Run Valgrind memory check
valgrind:
	valgrind --leak-check=full ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data 1
