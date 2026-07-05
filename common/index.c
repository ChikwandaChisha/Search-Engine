/**
 * index.c - implementation of the index module for Tiny Search Engine
 *
 * Author: Chikwanda Chisha
 * Date: 29 October, 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../libengine/hashtable.h"
#include "../libengine/counters.h"
#include "../libengine/mem.h"
#include "index.h"
#include "../libengine/file.h"

// Forward declarations for helper functions
static void save_count_helper(void *arg, const int docID, const int count);
static void save_word_helper(void *arg, const char *key, void *item);
static void delete_counter(void *item);

/**************** Helper Functions ****************/

static void save_count_helper(void *arg, const int docID, const int count) 
{
    FILE *fp = arg;
    fprintf(fp, " %d %d", docID, count);
}

static void save_word_helper(void *arg, const char *key, void *item) 
{
    FILE *fp = arg;
    counters_t *ctrs = item;
    
    // Write the word
    fprintf(fp, "%s", key);
    
    // Write all docID-count pairs for this word
    counters_iterate(ctrs, fp, save_count_helper);
    fprintf(fp, "\n");
}

static void delete_counter(void *item) 
{
    counters_delete(item);
}

/**************** index_new ****************/
index_t *
index_new(void)
{
    index_t *index = mem_malloc(sizeof(index_t));
    if (index == NULL) {
        return NULL;
    }
    
    index->table = hashtable_new(800);
    if (index->table == NULL) {
        mem_free(index);
        return NULL;
    }
    
    return index;
}

/**************** index_add ****************/
bool
index_add(index_t *index, const char *word, int docID)
{
    if (index == NULL || word == NULL || docID <= 0) {
        return false;
    }

    counters_t *ctrs = hashtable_find(index->table, word);
    if (ctrs == NULL) {
        ctrs = counters_new();
        if (ctrs == NULL) {
            return false;
        }
        
        if (!hashtable_insert(index->table, word, ctrs)) {
            counters_delete(ctrs);
            return false;
        }
    }
    
    counters_add(ctrs, docID);
    return true;
}

/**************** index_save ****************/
bool
index_save(index_t *index, const char *filename)
{
    if (index == NULL || filename == NULL) {
        return false;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        return false;
    }

    hashtable_iterate(index->table, fp, save_word_helper);
    fclose(fp);
    return true;
}

/**************** index_load ****************/
index_t *
index_load(const char *filename)
{
    if (filename == NULL) {
        return NULL;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }

    index_t *index = index_new();
    if (index == NULL) {
        fclose(fp);
        return NULL;
    }

    char *line;
    while ((line = file_readLine(fp)) != NULL) {
        char *word = strtok(line, " ");
        if (word == NULL) {
            mem_free(line);
            continue;
        }

        counters_t *ctrs = counters_new();
        if (ctrs == NULL) {
            mem_free(line);
            continue;
        }

        if (!hashtable_insert(index->table, word, ctrs)) {
            counters_delete(ctrs);
            mem_free(line);
            continue;
        }

        char *token;
        while ((token = strtok(NULL, " ")) != NULL) {
            int docID = atoi(token);
            token = strtok(NULL, " ");
            if (token == NULL) break;
            int count = atoi(token);
            counters_set(ctrs, docID, count);
        }
        mem_free(line);
    }

    fclose(fp);
    return index;
}

/**************** index_delete ****************/
void
index_delete(index_t *index)
{
    if (index == NULL) {
        return;
    }

    hashtable_delete(index->table, delete_counter);
    mem_free(index);
}

/**************** hashtable getter ****************/
hashtable_t *
index_get_hashtable(const index_t *index)
{
    return (index != NULL) ? index->table : NULL;
}