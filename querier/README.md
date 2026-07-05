# Querier

**Author:** Chikwanda Chisha  
**Date:** October 30, 2024  

Querier is a command-line program that takes user queries and retrieves matching documents from a specified set of crawled web pages, based on a provided index file. The program supports various types of queries, including basic, AND, OR, and complex queries. This README provides an overview of the program, installation instructions, and usage examples.

---

## Overview

The Querier program allows users to input search queries, which it uses to search through a specified directory of crawled web pages. It uses an index file to rank documents based on the relevance of each term in the query, displaying results in order of relevance.

## Features

- **Query Types**: Supports simple keywords, AND, OR, and mixed queries.
- **Error Handling**: Detects invalid queries, such as missing arguments, non-existent files, and invalid characters.
- **Memory Safety**: Uses `valgrind` to check for memory leaks during execution.
- **Ranking**: Scores and ranks documents based on relevance to the query.

## Installation
After cloning the repository, you should:

1. Compile the program:
   ```bash
   make querier
   ```

2. Ensure that the required directories and index files are set up as follows:
   - `pageDirectory`: Directory containing the crawled HTML pages.
   - `indexFilename`: The index file with word-document mappings.

## Usage

The basic command syntax for running the program is:

```bash
./querier <pageDirectory> <indexFilename>
```

### Examples

1. **Basic Query**:
   ```bash
   echo "computer" | ./querier ../data/quotes ../data/quotes.index
   ```

2. **AND Query**:
   ```bash
   echo "computer AND science" | ./querier ../data/quotes ../data/quotes.index
   ```

3. **OR Query**:
   ```bash
   echo "computer OR science" | ./querier ../data/quotes ../data/quotes.index
   ```

4. **Complex Query**:
   ```bash
   echo "computer AND science OR engineering" | ./querier ../data/quotes ../data/quotes.index
   ```

### Error Handling

The program handles errors for:
- Missing arguments
- Non-existent directory or file
- Invalid characters in queries
- Incorrect syntax in queries (e.g., adjacent `AND` and `OR` operators)

## Testing

Automated tests can be run to verify the functionality of querier. Run the following script to perform tests:

```bash
bash testing.sh
```

The tests check for:
- Argument validation
- Basic queries
- AND and OR queries
- Complex queries
- Invalid syntax and characters
- No-match cases
- Memory leaks using `valgrind`

### Example Output

The output of the test suite will display query results in a ranked order, along with any error messages for invalid inputs. Memory leaks (if any) will also be shown when using `valgrind`.




## Assumptions

- The pageDirectory passed as an argument was produced by the crawler and contains the required .crawler file.
- The indexFile passed as an argument was produced by the indexer.
- Document IDs in the index file are numbered sequentially from 1 to n.
- The provided URLs in document files are limited to a reasonable length (less than 500 characters).
- Input queries are limited to a reasonable length (less than 256 characters).

## Implementation Notes

- Implements a case-insensitive word matching system
- Handles memory management efficiently with no memory leaks
- Query words must be at least 3 characters long (as per indexer restrictions)
- Provides clear error messages for invalid queries
