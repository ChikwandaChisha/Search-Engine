# Chikwanda Chisha
# 30 October, 2024.

# Compiler settings
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common

# Object files for indexer and indextest
INDEXER_OBJS = indexer.o ../common/pagedir.o ../common/index.o ../common/word.o ../libcs50/file.o ../libcs50/libcs50-given.a
INDEXTEST_OBJS = indextest.o ../common/index.o ../libcs50/file.o ../libcs50/libcs50-given.a

# Valgrind settings for memory leak checking
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: all clean

# Default target builds both indexer and indextest
all: indexer indextest

# Compile indexer executable
indexer: $(INDEXER_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compile indextest executable
indextest: $(INDEXTEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compile indexer.o
indexer.o: indexer.c ../common/pagedir.h ../common/index.h ../common/word.h ../libcs50/file.h
	$(CC) $(CFLAGS) -c indexer.c

# Compile indextest.o
indextest.o: indextest.c ../common/index.h ../libcs50/file.h
	$(CC) $(CFLAGS) -c indextest.c

# Compile other required object files in common directory with -z noexecstack
../common/pagedir.o: ../common/pagedir.c ../common/pagedir.h
	$(CC) $(CFLAGS) -z noexecstack -c ../common/pagedir.c -o $@

../common/index.o: ../common/index.c ../common/index.h
	$(CC) $(CFLAGS) -z noexecstack -c ../common/index.c -o $@

../common/word.o: ../common/word.c ../common/word.h
	$(CC) $(CFLAGS) -z noexecstack -c ../common/word.c -o $@

../libcs50/file.o: ../libcs50/file.c ../libcs50/file.h
	$(CC) $(CFLAGS) -z noexecstack -c ../libcs50/file.c -o $@

# Run tests
test: indexer indextest testing.sh
	bash -v testing.sh > testing.out

# Valgrind for memory leak checks
valgrind: indexer indextest
	$(VALGRIND) ./indexer ~/cs50-dev/shared/tse/output/toscrape-1 ../data/toscrape.index
	$(VALGRIND) ./indextest ../data/toscrape.index ../data/toscrape.index_copy

# Clean up compiled files
clean:
	rm -f *~ *.o *.dSYM
	rm -f indexer
	rm -f indextest
	rm -f testing.out
