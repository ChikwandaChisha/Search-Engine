# Makefile for crawler program in CS50 TSE
# Chikwanda Chisha
# October 25, 2024


OBJS = crawler.o
LIBS = ../common/common.a ../libcs50/libcs50.a

# compiler options
CFLAGS = -Wall -pedantic -std=c11 -ggdb
INCLUDES = -I../libcs50 -I../common
CC = gcc
MAKE = make

.PHONY: all clean test

all: crawler

# crawler source dependencies
crawler.o: crawler.h crawler.c
	$(CC) $(CFLAGS) $(INCLUDES) -c crawler.c

# crawler binary dependencies
crawler: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o crawler

# Build the libraries
$(LIBS):
	$(MAKE) -C ../common
	$(MAKE) -C ../libcs50

test: crawler testing.sh
	bash -v testing.sh &> testing.out

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f crawler
	rm -f core