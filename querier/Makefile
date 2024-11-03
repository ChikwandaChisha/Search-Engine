# Makefile for querier
# Author: Chikwanda Chisha
# Date: 30 October, 2024

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
CPPFLAGS = -I../common -I../libcs50

# Libraries
LIBS = ../common/common.a ../common/index.o ../libcs50/libcs50.a  # Added ../common/index.o
LLIBS = -lm

# Default target
all: querier

# Querier program
querier: querier.o $(LIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) -o $@

querier.o: querier.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

.PHONY: clean test

# Clean up
clean:
	rm -f querier
	rm -f *.o
	rm -f *~
	rm -f core
	rm -f output*.txt testing.out

# Test target
test: querier testing.sh
	bash -v testing.sh &> testing.out
