# Tiny Search Engine - Crawler

## Author
Chikwanda Chisha  
Tiny Search Engine Project  
October 2024

## Overview
The **Crawler** program starts from a specified seed URL, explores all reachable pages within the same domain up to a given depth, and saves each page to a specified directory.

## Usage
To run the crawler:

```bash
./crawler seedURL pageDirectory maxDepth
```

- **seedURL**: Starting URL (must be from the local server).
- **pageDirectory**: Directory where pages will be saved (must exist and be writable).
- **maxDepth**: Maximum depth to crawl (between 0 and 10).

Example:
```bash
./crawler http://localhost:8080/index.html ../data 2
```

## Project Files
- **crawler.c**: Main crawler functionality.
- **crawler.h**: Header file for shared functions.
- **hashtable.c/.h**, **bag.c/.h**: Data structures for tracking pages.
- **webpage.c/.h**: Functions to fetch, parse, and normalize URLs.
- **pagedir.c/.h**: Handles saving pages to `pageDirectory`.

### Makefile
- **make**: Compiles the program.
- **make clean**: Cleans up binaries and object files.

## Running and Testing
### Compilation
```bash
make
```

### Basic Run
```bash
./crawler http://localhost:8080/index.html ../data 1
```

### Memory Check with Valgrind
To check for memory leaks:
```bash
valgrind --leak-check=full --show-leak-kinds=all ./crawler seedURL pageDirectory maxDepth
```

## Expected Output
Each page is saved in `pageDirectory` with a unique filename (`1`, `2`, `3`, ...), containing the HTML of each page.

