#!/bin/bash

# Chikwanda Chisha
# 30 October, 2024

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

# Test 1: Missing arguments
print_test_case "Missing arguments"
./indexer
./indexer $PAGE_DIR

# Test 2: Too many arguments
print_test_case "Too many arguments"
./indexer $PAGE_DIR $INDEX_FILE extra_arg

# Test 3: Invalid page directory
print_test_case "Invalid page directory"
./indexer invalid_directory $INDEX_FILE

# Test 4: Valid run with correct arguments
print_test_case "Valid run with correct arguments"
./indexer $PAGE_DIR $INDEX_FILE

# Check if the index file was created
if [ -f $INDEX_FILE ]; then
    echo "Index file created successfully."
else
    echo "Error: Index file was not created."
fi

# Test 5: Run indextest to check index integrity
print_test_case "Index integrity check with indextest"
./indextest $INDEX_FILE $INDEX_FILE_COPY

# Compare the original index file with the copy to ensure integrity
if cmp -s $INDEX_FILE $INDEX_FILE_COPY; then
    echo "Index integrity test passed: files are identical."
else
    echo "Index integrity test failed: files differ."
fi

# Test 6: Clean up and invalid directory (read-only example)
print_test_case "Invalid index file (read-only directory)"
mkdir -p readonly_dir && chmod 555 readonly_dir
./indexer $PAGE_DIR readonly_dir/test_index
chmod 755 readonly_dir && rm -rf readonly_dir

# Final message
echo -e "\n==== Testing Completed ===="
