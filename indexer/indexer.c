/*
 * indexer.c - Tiny Search Engine Indexer
 *
 * This program builds an inverted index from files in a specified page directory
 * (produced by the TSE crawler) and saves it to an output file. The index maps words
 * to document IDs and occurrence counts, which is later used by a search engine querier.
 *
 * Exit Status:
 * - 0 on success, non-zero on error with appropriate error messages to stderr.
 *
 * Author: Chikwanda Chisha
 * Date: October 26,
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../libengine/webpage.h"
#include "../libengine/hashtable.h"
#include "../libengine/counters.h"
#include "../libengine/bag.h"
#include "../common/pagedir.h"
#include "../libengine/mem.h"
#include "../common/index.h"
#include "../common/word.h"
#include "../libengine/file.h"   // Added for file utilities

// Function declarations
bool indexBuild(const char *pageDirectory, index_t *index);
void indexPage(webpage_t *page, int docID, index_t *index);

int main(int argc, char *argv[]) {
    // Ensure exactly 2 arguments (pageDirectory and indexFilename) are provided
    if (argc != 3) {
        fprintf(stderr, "Usage: ./indexer pageDirectory indexFilename\n");
        return 1;
    }
    
    char *pageDirectory = argv[1];
    char *indexFilename = argv[2];
    
    // Validate that the pageDirectory is a valid Crawler-produced directory
    if (!pagedir_validate(pageDirectory)) {
        fprintf(stderr, "Error: Invalid page directory\n");
        return 1;
    }

    // Initialize the index structure with a reasonable hashtable size (800 slots)
    index_t *index = index_new();  
    if (index == NULL) {
        fprintf(stderr, "Error: Could not initialize index\n");
        return 1;
    }

    // Build the index from the documents in pageDirectory
    if (!indexBuild(pageDirectory, index)) {
        fprintf(stderr, "Error: Failed to build index\n");
        index_delete(index);
        return 1;
    }

    // Save the constructed index to the specified index file
    if (!index_save(index, indexFilename)) {
        fprintf(stderr, "Error: Could not save index to file\n");
        index_delete(index);
        return 1;
    }

    // Clean up allocated memory for the index before exiting
    index_delete(index);
    return 0;
}

// Builds the index by reading files from pageDirectory, one by one
bool indexBuild(const char *pageDirectory, index_t *index) {
    int docID = 1;        // Start with document ID 1
    char filepath[1024];  // Buffer for constructing file paths
    
    // Loop through documents in the pageDirectory, starting from document 1
    while (1) {
        snprintf(filepath, sizeof(filepath), "%s/%d", pageDirectory, docID);
        
        // Load the webpage from the file, stopping if it can't be loaded
        webpage_t *page = pagedir_load(filepath);
        if (page == NULL) {
            // Break if no more pages can be loaded (end of sequence)
            break;
        }

        // Add the page's words to the index using indexPage function
        indexPage(page, docID, index);

        // Free the loaded webpage and move to the next document ID
        webpage_delete(page);
        docID++;
    }
    return true;
}

// Processes a single webpage, adding each valid word to the index
void indexPage(webpage_t *page, int docID, index_t *index) {
    int pos = 0;       // Initialize position in the document
    char *word;        // Pointer to hold each word found
    
    // Extract each word from the page
    while ((word = webpage_getNextWord(page, &pos)) != NULL) {
        if (strlen(word) >= 3) {         // Only index words with 3 or more characters
            normalizeWord(word);         // Convert the word to lowercase
            index_add(index, word, docID);  // Add or update the word in the index
        }
        free(word);  // Free the dynamically allocated word
    }
}
