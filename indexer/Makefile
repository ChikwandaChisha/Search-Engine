# Makefile for 'indexer' module
#
# Chikwanda Chisha, October 30, 2024

LIBS = ../libcs50/libcs50-given.a
LLIBS = ../libcs50/file.o ../libcs50/libcs50-given.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common $(TESTING)
CC = gcc
MAKE = make

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

OBJS = indexer.o ../common/pagedir.o ../common/index.o ../common/word.o ../libcs50/file.o
TOBJS = indextest.o ../common/index.o ../common/pagedir.o ../common/word.o ../libcs50/file.o

.PHONY: all clean test valgrind

all: indexer indextest

indexer: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o indexer

indextest: $(TOBJS) $(LIBS)
	$(CC) $(CFLAGS) $(TOBJS) $(LIBS) -o indextest

test: indexer indextest testing.sh
	bash -v testing.sh

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f indexer
	rm -f indextest
	rm -f core