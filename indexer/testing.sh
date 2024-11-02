#!/bin/bash

# Author: Chikwanda Chisha
# Date: 30 October 2024
# Description: Script to test the indexer and indextest programs for various scenarios

# Set directories and files for testing
PAGE_DIR="../data/test_pages"
INDEX_FILE="../data/test_index"
INDEX_FILE_COPY="../data/test_index_copy"

# Clean up any previous output files
rm -f $INDEX_FILE $INDEX_FILE_COPY

# Helper function to print test case headers
print_test_case() {
    echo -e "\n==== Test Case: $1 ===="
}

# Ensure indexer and indextest are compiled and available
if [ ! -x ./indexer ] || [ ! -x ./indextest ]; then
    echo "Error: indexer or indextest executable not found. Please compile them first."
    exit 1
fi

# Test 1: Missing arguments
print_test_case "Missing arguments"
./indexer 2>&1 | grep -q "Usage: ./indexer pageDirectory indexFilename" && echo "Pass" || echo "Fail"
./indexer $PAGE_DIR 2>&1 | grep -q "Usage: ./indexer pageDirectory indexFilename" && echo "Pass" || echo "Fail"

# Test 2: Too many arguments
print_test_case "Too many arguments"
./indexer $PAGE_DIR $INDEX_FILE extra_arg 2>&1 | grep -q "Usage: ./indexer pageDirectory indexFilename" && echo "Pass" || echo "Fail"

# Test 3: Invalid page directory
print_test_case "Invalid page directory"
./indexer invalid_directory $INDEX_FILE 2>&1 | grep -q "Error: Invalid page directory" && echo "Pass" || echo "Fail"

# Test 4: Valid run with correct arguments
print_test_case "Valid run with correct arguments"
./indexer $PAGE_DIR $INDEX_FILE 2>&1
# Check if the index file was created successfully
if [ -f $INDEX_FILE ]; then
    echo "Index file created successfully."
else
    echo "Error: Index file was not created."
fi

# Test 5: Run indextest to check index integrity
print_test_case "Index integrity check with indextest"
./indextest $INDEX_FILE $INDEX_FILE_COPY 2>&1 | grep -q "could not load index" && echo "Error: Index integrity test failed." || echo "Index integrity test passed."

# Compare the original index file with the copy to ensure integrity
if cmp -s $INDEX_FILE $INDEX_FILE_COPY; then
    echo "Index integrity test passed: files are identical."
else
    echo "Index integrity test failed: files differ."
fi

# Test 6: Invalid index file (read-only directory)
print_test_case "Invalid index file (read-only directory)"
mkdir -p readonly_dir && chmod 555 readonly_dir
# We expect "Error: Could not save index to file"
./indexer $PAGE_DIR readonly_dir/test_index 2>&1 | grep -q "Error: Could not save index to file" && echo "Pass" || echo "Fail"
chmod 755 readonly_dir && rm -rf readonly_dir

# Final message
echo -e "\n==== Testing Completed ===="