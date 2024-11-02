/*
 * indextest.c - test program for TSE indexer module
 *
 * Usage: ./indextest oldIndexFilename newIndexFilename
 * 
 * This program loads an index file, saves it to a new file,
 * and ensures the contents match.
 *
 * Author: Chikwanda Chisha
 * Date: October 29, 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/mem.h"
#include "../libcs50/file.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../common/index.h"

int main(int argc, char *argv[]) 
{
    // Check command line arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s oldIndexFilename newIndexFilename\n", argv[0]);
        return 1;
    }

    char *oldIndexFilename = argv[1];
    char *newIndexFilename = argv[2];

    // Load the old index
    index_t *index = index_load(oldIndexFilename);
    if (index == NULL) {
        fprintf(stderr, "Error: Could not load index from %s\n", oldIndexFilename);
        return 1;
    }

    // Save to the new file
    if (!index_save(index, newIndexFilename)) {
        fprintf(stderr, "Error: Could not save index to %s\n", newIndexFilename);
        index_delete(index);
        return 1;
    }

    // Clean up
    index_delete(index);
    return 0;
}