/**
 * index.h - header file for the index module for Tiny Search Engine
 *
 * Author: Chikwanda Chisha
 * Date: 29 October, 2024.
 */

#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>
#include <stdbool.h>
#include "../libengine/counters.h"
#include "../libengine/hashtable.h"

/**
 * Index structure, representing an in-memory inverted index.
 * Maps words to a list of (docID, count) pairs where `count` represents the 
 * number of times the word appears in the document with the specified docID.
 */
typedef struct index {
    hashtable_t *table;  // Hashtable that maps words to counters
} index_t;

/**
 * Creates a new index.
 * 
 * Allocates memory for an index and initializes it. The index uses a hashtable
 * where each key is a word, and each value is a counter set with document IDs
 * and word counts.
 * 
 * Returns:
 *   pointer to a new index, or NULL if memory allocation fails.
 */
index_t *index_new(void);

/**
 * Adds or updates a word occurrence in the index.
 * 
 * If `word` is not already in the index, it is added with an initial count
 * for `docID`. If `word` is already in the index, it increments the count
 * for the specified `docID`.
 * 
 * Parameters:
 *   index - pointer to the index structure
 *   word - string representing the word to index
 *   docID - integer document ID where the word was found
 * 
 * Returns:
 *   true if successful, false if an error occurs (e.g., memory allocation failure).
 */
bool index_add(index_t *index, const char *word, int docID);

/**
 * Saves the index to a file.
 * 
 * Writes the index to `filename`, where each line contains a word followed by 
 * a list of (docID, count) pairs. This format allows for easy loading in the future.
 * 
 * Parameters:
 *   index - pointer to the index to save
 *   filename - name of the file to save the index to
 * 
 * Returns:
 *   true if the index was saved successfully, false if an error occurs.
 */
bool index_save(index_t *index, const char *filename);

/**
 * Loads an index from a file.
 * 
 * Reads an index from `filename` and populates it into a new index structure.
 * Assumes the file format matches the format used by `index_save`.
 * 
 * Parameters:
 *   filename - name of the file to read the index from
 * 
 * Returns:
 *   a pointer to the loaded index, or NULL if an error occurs.
 */
index_t *index_load(const char *filename);

/**
 * Deletes an index, freeing all allocated memory.
 * 
 * Frees the index and all associated data, including words and their
 * document count mappings.
 * 
 * Parameters:
 *   index - pointer to the index to delete
 */
void index_delete(index_t *index);

/**
 * Returns the hashtable contained in the index.
 * 
 * Parameters:
 *   index - pointer to the index
 * 
 * Returns:
 *   pointer to the hashtable, or NULL if index is NULL
 */
hashtable_t *index_get_hashtable(const index_t *index);

#endif // INDEX_H