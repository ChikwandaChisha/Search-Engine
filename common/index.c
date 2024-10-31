/**
 * index.c - implementation of the index module for Tiny Search Engine
 *
 * This module provides functions for creating, adding to, saving, loading, 
 * and deleting an index data structure, which stores an inverted index 
 * mapping words to document occurrences.
 * 
 * Author: Chikwanda Chisha
 * Date: 29 October,
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../libcs50/mem.h"
#include "index.h"
#include "../libcs50/set.h"
#include "../libcs50/file.h"

typedef struct index {
    hashtable_t *table;  // Hashtable mapping words to counters
} index_t;

/**************** index_new ****************/
index_t *index_new(void) {
    index_t *index = mem_malloc(sizeof(index_t));
    if (index == NULL) {
        return NULL;  // Memory allocation failure
    }
    index->table = hashtable_new(500);  // Initialize with a default size
    if (index->table == NULL) {
        mem_free(index);
        return NULL;
    }
    return index;
}

/**************** index_add ****************/
bool index_add(index_t *index, const char *word, int docID) {
    if (index == NULL || word == NULL || docID <= 0) return false;

    counters_t *counter = hashtable_find(index->table, word);
    if (counter == NULL) {
        // Word not found in index, create a new counter
        counter = counters_new();
        if (counter == NULL) return false;  // Memory allocation failure
        if (!hashtable_insert(index->table, word, counter)) {
            counters_delete(counter);
            return false;
        }
    }
    // Increment the count for this docID
    counters_add(counter, docID);
    return true;
}

/**************** index_save ****************/
bool index_save(index_t *index, const char *filename) {
    if (index == NULL || filename == NULL) return false;

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) return false;  // File open failure

    // Helper function to write each word and its counts
    void index_save_helper(void *arg, const char *key, void *item) {
        FILE *fp = arg;
        counters_t *counter = item;
        
        // Print the word
        fprintf(fp, "%s", key);

        // Print each (docID, count) pair
        void print_counter_helper(void *arg, const int docID, const int count) {
            FILE *fp = arg;
            fprintf(fp, " %d %d", docID, count);
        }
        counters_iterate(counter, fp, print_counter_helper);
        fprintf(fp, "\n");
    }

    hashtable_iterate(index->table, fp, index_save_helper);
    fclose(fp);
    return true;
}

/**************** index_load ****************/
index_t *index_load(const char *filename) {
    if (filename == NULL) return NULL;

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return NULL;  // File open failure

    index_t *index = index_new();
    if (index == NULL) {
        fclose(fp);
        return NULL;
    }

    char word[200];
    int docID, count;

    while (fscanf(fp, "%s", word) != EOF) {
        counters_t *counter = counters_new();
        if (counter == NULL) {
            index_delete(index);
            fclose(fp);
            return NULL;
        }
        hashtable_insert(index->table, word, counter);

        while (fscanf(fp, "%d %d", &docID, &count) == 2) {
            counters_set(counter, docID, count);
        }
    }
    fclose(fp);
    return index;
}

/**************** index_delete ****************/
void index_delete(index_t *index) {
    if (index == NULL) return;

    // Helper function to delete each counter in the hashtable
    void index_delete_helper(void *item) {
        counters_t *counter = item;
        counters_delete(counter);
    }
    hashtable_delete(index->table, index_delete_helper);
    mem_free(index);
}
