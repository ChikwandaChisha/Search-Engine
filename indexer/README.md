# Tiny Search Engine Indexer
Fall 2024, Chikwanda Chisha  
Check `IMPLEMENTATION.md` for implementation details.

## Indexer
The TSE indexer is a standalone program that reads the document files produced by the TSE crawler, builds an index, and writes that index to a file. Its companion, the index tester, loads an index file produced by the indexer and saves it to another file for verification and testing purposes.

## Usage
The `indexer` program can be run as:

```bash
./indexer pageDirectory indexFilename
```

- **pageDirectory**: Path to a directory produced by the TSE crawler, containing files with sequential document IDs.
- **indexFilename**: Path to the file in which the index will be written. This file is created if it does not exist, or overwritten if it already exists.

The `indexer` program is implemented in `indexer.c` as follows:

```c
int main(int argc, char *argv[]);
```

The `indextest` program can be run as:

```bash
./indextest oldIndexFilename newIndexFilename
```

- **oldIndexFilename**: Path to an index file produced by the `indexer` program.
- **newIndexFilename**: Path to the file where the loaded index will be saved, overwriting it if it already exists.

The `indextest` program is implemented in `indextest.c` as follows:

```c
/**************** main ****************/
/* The main function performs the following tasks:
 * - Validates arguments to ensure correct usage.
 * - Calls the `test` function to load and save the index.
 */
int main(int argc, char *argv[]);

/**************** test ****************/
/* Loads the index from `oldIndexFilename` into an inverted-index data structure.
 * Writes the index to `newIndexFilename` using the specified format, to verify the index's consistency.
 */
void test(char *oldIndexFilename, char *newIndexFilename);
```

## Assumptions
- **indexer.c**: Assumes `pageDirectory` has files named sequentially from 1, 2, 3, ..., without any gaps. The contents of files in `pageDirectory` follow the crawler-specified format.
- **indextest.c**: Assumes the content of the index file matches the format specified in the indexer requirements.
- **Error Handling**: The indexer and indextest handle errors such as missing arguments, invalid directories, and file write permissions.

## Exit Codes
- `0` - Program executed successfully.
- `1` - Incorrect number of arguments.
- `2` - Directory specified is not a crawler-produced directory.
- `3` - Unable to write to file or open directory.
- `4` - Failed to create a new webpage object.
- `5` - Memory allocation failure (malloc failure).

## Compilation
- To compile both `indexer` and `indextest`, use:

  ```bash
  make
  ```

- To clean up object files and executables, use:

  ```bash
  make clean
  ```

- To run tests and validate functionality, use:

  ```bash
  make test
  ```

Output from the test script is stored in `testing.out`, where you can check for results of various cases. For additional details on testing and implementation, refer to `testing.sh` and `IMPLEMENTATION.md`.






