#!/bin/bash
# testing.sh - Automated tests for querier
# Author: Chikwanda Chisha
# Date: 30 October, 2024

# Define path to test data
TESTDIR=~/cs50-dev/shared/tse/output/toscrape-2
INDEXFILE=~/cs50-dev/shared/tse/output/toscrape-2.index

echo "Starting querier tests..."

# Test 1: Invalid arguments
echo "Testing invalid arguments..."
./querier                                    # no arguments
./querier $TESTDIR                          # missing second argument
./querier $TESTDIR $INDEXFILE extraarg      # extra argument
./querier /nonexistent $INDEXFILE           # nonexistent directory
./querier $TESTDIR /nonexistent/index       # nonexistent file

# Test 2: Basic queries
echo "Testing basic queries..."
echo "computer" | ./querier $TESTDIR $INDEXFILE
echo "science" | ./querier $TESTDIR $INDEXFILE
echo "programming" | ./querier $TESTDIR $INDEXFILE

# Test 3: AND queries
echo "Testing AND queries..."
echo "computer science" | ./querier $TESTDIR $INDEXFILE
echo "computer AND science" | ./querier $TESTDIR $INDEXFILE
echo "computer and programming" | ./querier $TESTDIR $INDEXFILE

# Test 4: OR queries
echo "Testing OR queries..."
echo "computer OR science" | ./querier $TESTDIR $INDEXFILE
echo "programming OR engineering" | ./querier $TESTDIR $INDEXFILE

# Test 5: Complex queries
echo "Testing complex queries..."
echo "computer AND science OR engineering" | ./querier $TESTDIR $INDEXFILE
echo "computer science AND programming OR software" | ./querier $TESTDIR $INDEXFILE

# Test 6: Invalid syntax
echo "Testing invalid syntax..."
echo "and computer" | ./querier $TESTDIR $INDEXFILE
echo "computer or" | ./querier $TESTDIR $INDEXFILE
echo "computer and or science" | ./querier $TESTDIR $INDEXFILE
echo "computer and and science" | ./querier $TESTDIR $INDEXFILE

# Test 7: Invalid characters
echo "Testing invalid characters..."
echo "computer!" | ./querier $TESTDIR $INDEXFILE
echo "(computer)" | ./querier $TESTDIR $INDEXFILE
echo "computer@science" | ./querier $TESTDIR $INDEXFILE

# Test 8: Empty and special cases
echo "Testing empty and special cases..."
echo "" | ./querier $TESTDIR $INDEXFILE
echo "   " | ./querier $TESTDIR $INDEXFILE
echo "a" | ./querier $TESTDIR $INDEXFILE        # single letter word
echo "an" | ./querier $TESTDIR $INDEXFILE       # two letter word

# Test 9: No Match Cases
echo "No Match Cases..."
echo "xaerhieuyhfbi" | ./querier $TESTDIR $INDEXFILE
echo "BEHGRIOHGIrierfy" | ./querier $TESTDIR $INDEXFILE

# Test 10: Memory test
echo "Testing for memory leaks..."
echo "computer science" | valgrind --leak-check=full --show-leak-kinds=all ./querier $TESTDIR $INDEXFILE 2>&1 | grep "definitely lost"

echo "All tests complete."
