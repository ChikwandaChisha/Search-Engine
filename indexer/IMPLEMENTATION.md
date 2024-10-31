### `IMPLEMENTATION.md`

# Tiny Search Engine Indexer - Implementation Details

## Overview
The `indexer` program reads crawled documents, builds an inverted index mapping words to documents, and saves it to a file. `indextest` loads this file, tests its structure, and saves it to a new file to verify integrity.

## Program Structure

### `indexer.c`
- **`main`**: Validates input, initializes the index, builds the index from documents, and saves it to `indexFilename`.
- **`indexBuild`**: Reads documents by ID in `pageDirectory`, adding each to the index.
- **`indexPage`**: Processes each word in a document, normalizes it, and adds it to the index with its document ID and count.

### `indextest.c`
- **`main`**: Validates input and calls `test`.
- **`test`**: Loads `oldIndexFilename` into memory, then saves it to `newIndexFilename` for validation.

## Key Modules

### `index.c` and `index.h`
- **Structure**: `index_t` stores a hashtable mapping words to counters of document IDs and counts.
- **Main Functions**: `index_new`, `index_add`, `index_save`, `index_load`, and `index_delete`.

### `pagedir.c` and `pagedir.h`
- **Functions**: `pagedir_validate` (checks directory format) and `pagedir_load` (reads document files).

### `word.c` and `word.h`
- **Function**: `normalizeWord` (converts words to lowercase).

## Index File Format
Each line lists a word followed by `(docID, count)` pairs, e.g., `word 1 3 2 1`.

## Error Handling and Assumptions
- **Exit Codes**: 0 (Success), 1-5 for errors (invalid args, directory, etc.).
- **Assumptions**: Sequential document IDs, lowercase normalization, and words >2 characters only.

