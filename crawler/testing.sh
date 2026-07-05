#!/bin/bash

# Directory for storing test outputs
output_dir="../data"

# Ensure the output directory exists; create it if not
mkdir -p $output_dir

# Run tests for various edge cases and conditions

echo "Running Test 1: Valid seed URL, depth 0"
# Test with a valid seed URL and minimum depth (0) to verify shallow crawl
./crawler http://localhost:8080/index.html $output_dir 0

echo "Running Test 2: Valid seed URL, depth 1"
# Test with valid seed URL and depth of 1 for basic crawl
./crawler http://localhost:8080/index.html $output_dir 1

echo "Running Test 3: Valid seed URL, maximum depth 10"
# Test with valid seed URL at maximum depth limit (10) for deep crawl
./crawler http://localhost:8080/index.html $output_dir 10

echo "Running Test 4: Invalid seed URL (external URL)"
# Test with an external URL, which should raise an error as it's outside allowed domain
./crawler http://example.com $output_dir 1

echo "Running Test 5: Non-existent page directory"
# Test with a directory that doesn’t exist to check handling of invalid directory paths
./crawler http://localhost:8080/index.html nonexistent_dir 1

echo "Running Test 6: Invalid maxDepth (-1)"
# Test with an invalid maxDepth value (-1), which should produce an error
./crawler http://localhost:8080/index.html $output_dir -1

echo "Running Test 7: Duplicate URL handling"
# Test with a URL that may produce duplicates, to confirm duplicates are ignored
./crawler http://localhost:8080/index.html $output_dir 1

echo "Running Test 8: Empty seed URL"
# Test with an empty seed URL, which should trigger a usage or error message
./crawler "" $output_dir 1

echo "Running Test 9: Non-integer maxDepth (\"five\")"
# Test with a non-integer maxDepth argument to verify error handling of input types
./crawler http://localhost:8080/index.html $output_dir five

echo "Running Test 10: Unreachable URL (404)"
# Test with a URL that leads to a non-existent page (404 error) to check error handling
./crawler http://localhost:8080/nonexistent.html $output_dir 1

echo "Running Test 11: Circular links"
# Test with a page that links back to itself, creating a potential crawl loop
# This assumes you have set up a circular_link.html that links to itself or in a loop
./crawler http://localhost:8080/circular_link.html $output_dir 2

echo "Running Test 12: Limited pages, large maxDepth"
# Use a seed URL with few links and set a large maxDepth to check depth limits without excess pages
./crawler http://localhost:8080/index.html $output_dir 10

echo "Running Test 13: Non-HTML resource link"
# Test with a URL pointing to a non-HTML resource (e.g., a PDF) to check handling of unsupported formats
./crawler http://localhost:8080/sample.pdf $output_dir 1

# Clean up test output by removing all files created in the data directory
rm -rf $output_dir/*
