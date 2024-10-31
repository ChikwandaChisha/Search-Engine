/*
 * indextest.c - Tiny Search Engine Index Test
 *
 * This program loads an index file produced by the indexer, saves it to a new file, 
 * and verifies that it can be read and written in the expected format.
 *
 * Author: Chikwanda Chisha
 * Date: 28 October, 2024.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../common/index.h"
#include "../common/pagedir.h"
#include "../libcs50/file.h"

static void test(const char* oldIndexFilename, const char* newIndexFilename);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: ./indextest oldIndexFilename newIndexFilename\n");
        exit(1);
    }

    const char* oldIndexFilename = argv[1];
    const char* newIndexFilename = argv[2];

    test(oldIndexFilename, newIndexFilename);

    exit(0);
}

/**************** test() ****************/
/* Loads an index from oldIndexFilename, writes it to newIndexFilename, 
 * and deletes the in-memory index after testing.
 */
static void test(const char* oldIndexFilename, const char* newIndexFilename) {
    // Initialize a new index
    index_t* index = index_new();
    if (index == NULL) {
        fprintf(stderr, "Error: could not initialize index\n");
        exit(2);
    }

    // Load the index data from the old index file
    if (!index_load(oldIndexFilename)) { // Pass filename directly
        fprintf(stderr, "Error: could not load index from %s\n", oldIndexFilename);
        index_delete(index);
        exit(3);
    }

    // Write the loaded index to the new index file
    if (!index_save(index, newIndexFilename)) { // Use index_save instead of index_writeToFile
        fprintf(stderr, "Error: could not write index to %s\n", newIndexFilename);
        index_delete(index);
        exit(5);
    }

    // Clean up and delete the in-memory index
    index_delete(index);
}
