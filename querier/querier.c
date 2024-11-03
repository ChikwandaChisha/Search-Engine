/**********************************
 * querier.c - Tiny Search Engine Querier
 *
 * CS50 Lab 6 - Querier
 * Processes search queries using an inverted index.
 *
 * Usage: ./querier pageDirectory indexFilename
 *
 * Author: Chikwanda Chisha
 * Date: 30 October, 2024
 **********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include "hashtable.h"
#include "pagedir.h"
#include "word.h"
#include "counters.h"
#include "../libcs50/file.h"
#include "../common/index.h"

// Structure for tracking document scores
typedef struct doc_score {
    int docID;
    int score;
} doc_score_t;

typedef struct fill_array_args {
    doc_score_t *scores;    // Pointer to the array of doc_score_t
    int current_index;      // Current index for insertion in the array
    int array_size;         // Maximum capacity of the scores array
} fill_array_args_t;


// Function prototypes
void process_query(char *query, index_t *index, const char *pageDirectory);
char *clean_input(const char *query);
char **tokenize(char *query);
counters_t *evaluate_query(char **tokens, index_t *index);
void print_results(counters_t *results, const char *pageDirectory);

// Helper function prototypes
static bool validate_query_syntax(char **tokens);
static void count_nonzero(void *arg, const int key, const int count);
static void fill_array(void *arg, const int key, const int count);
static void intersect_counters(counters_t *c1, counters_t *c2);
static void intersect_helper(void *arg, const int key, const int count);
static void union_counters(counters_t *c1, counters_t *c2);
static void union_helper(void *arg, const int key, const int count);



int main(int argc, char *argv[]) {
    // Validate command-line arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: ./querier pageDirectory indexFilename\n");
        return 1;
    }

    char *pageDirectory = argv[1];
    char *indexFilename = argv[2];

    // Check if pageDirectory is valid
    if (!pagedir_validate(pageDirectory)) {
        fprintf(stderr, "Error: invalid pageDirectory.\n");
        return 1;
    }

    // Load the index from the specified index file
    index_t *index = index_load(indexFilename);
    if (index == NULL) {
        fprintf(stderr, "Error: failed to load index from %s\n", indexFilename);
        return 1;
    }

    // Interactive prompt loop for user queries
    char query[256];
    while (true) {
        // Prompt user for a query (only if stdin is a terminal)
        if (isatty(STDIN_FILENO)) {
            printf("Query? ");
        }

        if (fgets(query, sizeof(query), stdin) == NULL) {
            break;  // Exit on EOF
        }

        process_query(query, index, pageDirectory);  // Process each query
    }

    // Free the index structure and all its components
    index_delete(index);

    return 0;  // Successful exit
}

/*
 * process_query - Cleans, tokenizes, and evaluates a query.
 */
void process_query(char *query, index_t *index, const char *pageDirectory) {
    // Clean the input query and convert to lowercase
    char *cleanQuery = clean_input(query);
    if (cleanQuery == NULL || strlen(cleanQuery) == 0) {
        free(cleanQuery);
        return;  // Exit if query is empty or invalid
    }

    printf("Query: %s\n", cleanQuery);  // Print the cleaned query

    // Tokenize the cleaned query into an array of words
    char **tokens = tokenize(cleanQuery);
    if (tokens == NULL) {
        free(cleanQuery);
        return;  // Exit if tokenization failed
    }

    // Evaluate the tokenized query to get matching documents
    counters_t *results = evaluate_query(tokens, index);

    // Print the results if any documents matched the query
    if (results != NULL) {
        print_results(results, pageDirectory);  // Display results in rank order
        counters_delete(results);  // Free results counters
    }

    // Free each token and the tokens array itself
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);

    // Free the cleaned query string
    free(cleanQuery);
}

/*
 * clean_input - Returns a cleaned version of the query string.
 */
char *clean_input(const char *query) {
    if (query == NULL) return NULL;
    
    // Allocate space for cleaned query
    char *cleaned = malloc(strlen(query) + 1);
    if (cleaned == NULL) return NULL;
    
    int j = 0;
    bool lastWasSpace = true;  // Track if last char was space
    
    // Process each character
    for (int i = 0; query[i] != '\0' && query[i] != '\n'; i++) {
        if (isalpha(query[i])) {
            cleaned[j++] = tolower(query[i]);
            lastWasSpace = false;
        }
        else if (isspace(query[i])) {
            // Add only one space between words
            if (!lastWasSpace) {
                cleaned[j++] = ' ';
                lastWasSpace = true;
            }
        }
        else {
            // Invalid character found
            fprintf(stderr, "Error: bad character '%c' in query.\n", query[i]);
            free(cleaned);
            return NULL;
        }
    }
    
    // Remove trailing space if exists
    if (j > 0 && cleaned[j-1] == ' ') {
        j--;
    }
    cleaned[j] = '\0';
    
    return cleaned;
}

/*
 * tokenize - Splits the query string into words.
 */
char **tokenize(char *query) {
    if (query == NULL) return NULL;
    
    // Count words to allocate array
    int wordCount = 1;  // Start at 1 for the first word
    for (char *p = query; *p != '\0'; p++) {
        if (*p == ' ') wordCount++;
    }
    
    // Allocate array of string pointers
    char **tokens = malloc((wordCount + 1) * sizeof(char *));  // +1 for NULL terminator
    if (tokens == NULL) return NULL;
    
    // Split string into tokens
    int i = 0;
    char *token = strtok(query, " ");
    while (token != NULL) {
        tokens[i] = malloc(strlen(token) + 1);
        if (tokens[i] == NULL) {
            // Clean up on error
            for (int j = 0; j < i; j++) {
                free(tokens[j]);
            }
            free(tokens);
            return NULL;
        }
        strcpy(tokens[i], token);
        i++;
        token = strtok(NULL, " ");
    }
    tokens[i] = NULL;  // NULL terminate array
    
    // Validate query syntax
    if (!validate_query_syntax(tokens)) {
        // Clean up on invalid syntax
        for (int j = 0; j < i; j++) {
            free(tokens[j]);
        }
        free(tokens);
        return NULL;
    }
    
    return tokens;
}

/*
 * Helper function to validate query syntax
 */
static bool validate_query_syntax(char **tokens) {
    if (tokens == NULL || tokens[0] == NULL) return false;
    
    // Check first and last words aren't operators
    if (strcmp(tokens[0], "and") == 0 || strcmp(tokens[0], "or") == 0) {
        fprintf(stderr, "Error: '%s' cannot be first\n", tokens[0]);
        return false;
    }
    
    // Count tokens and check last word
    int i;
    for (i = 0; tokens[i] != NULL; i++);
    if (strcmp(tokens[i-1], "and") == 0 || strcmp(tokens[i-1], "or") == 0) {
        fprintf(stderr, "Error: '%s' cannot be last\n", tokens[i-1]);
        return false;
    }
    
    // Check for adjacent operators
    for (i = 1; tokens[i] != NULL; i++) {
        if ((strcmp(tokens[i-1], "and") == 0 || strcmp(tokens[i-1], "or") == 0) &&
            (strcmp(tokens[i], "and") == 0 || strcmp(tokens[i], "or") == 0)) {
            fprintf(stderr, "Error: '%s' and '%s' cannot be adjacent\n", 
                    tokens[i-1], tokens[i]);
            return false;
        }
    }
    
    return true;
}

/*
 * evaluate_query - Processes tokens and evaluates the query.
 */
counters_t *evaluate_query(char **tokens, index_t *index) {
    if (tokens == NULL || tokens[0] == NULL || index == NULL) return NULL;

    // Retrieve the hashtable from the index for fast word lookups
    hashtable_t *ht = index_get_hashtable(index);
    if (ht == NULL) return NULL;

    // Initialize the final results counters to store document scores
    counters_t *result = counters_new();
    if (result == NULL) return NULL;

    counters_t *current_and = NULL;  // Temporary counter for AND sequences
    bool is_first = true;  // Flag for first word in an AND sequence

    // Iterate through each token in the query
    for (int i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "or") == 0) {
            // 'or' signifies the end of an AND sequence
            is_first = true;
            continue;
        }

        if (strcmp(tokens[i], "and") == 0) continue;  // Ignore 'and' tokens

        // Get counters for the current word from the hashtable
        counters_t *word_counters = hashtable_find(ht, tokens[i]);

        if (is_first) {
            // First word in the AND sequence initializes current_and counters
            current_and = counters_new();
            if (current_and != NULL && word_counters != NULL) {
                // Copy word counters to current_and if they exist
                counters_iterate(word_counters, current_and, union_helper);
            }
            is_first = false;  // Reset first word flag
        } else if (tokens[i+1] != NULL && strcmp(tokens[i+1], "or") != 0) {
            // Further AND words: intersect current_and with word counters
            if (word_counters != NULL) {
                intersect_counters(current_and, word_counters);
            } else {
                // Word not found: reset current_and to an empty set
                counters_delete(current_and);
                current_and = counters_new();
            }
        } else {
            // Last word in the AND sequence, perform union with result
            if (word_counters != NULL) {
                intersect_counters(current_and, word_counters);
            }
            union_counters(result, current_and);  // Union into final result
            counters_delete(current_and);  // Free temporary AND sequence counters
            current_and = NULL;  // Reset current_and for the next sequence
            is_first = true;  // Reset first word flag for next sequence
        }
    }

    // Ensure any remaining AND sequence is unioned into the result
    if (current_and != NULL) {
        union_counters(result, current_and);
        counters_delete(current_and);  // Free the last AND sequence
    }

    return result;  // Return final document scores
}


/*
 * print_results - Prints the ranked query results.
 */
#include <stdlib.h> // Add this for qsort if not already included

// Comparator function for qsort to sort by score in descending order
int compare_scores(const void *a, const void *b) {
    doc_score_t *scoreA = (doc_score_t *)a;
    doc_score_t *scoreB = (doc_score_t *)b;
    return scoreB->score - scoreA->score;
}

/**
 * Function to print the results in rank order based on document scores.
 */
void print_results(counters_t *results, const char *pageDirectory) {
    if (results == NULL || pageDirectory == NULL) return;

    // Step 1: Count number of non-zero results
    int num_results = 0;
    counters_iterate(results, &num_results, count_nonzero);

    if (num_results == 0) {
        printf("No documents match.\n");
        return;
    }

    // Step 2: Allocate memory for an array of doc_score_t to hold the scores
    doc_score_t *scores = calloc(num_results, sizeof(doc_score_t));
    if (scores == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in print_results.\n");
        return;
    }

    // Step 3: Initialize the fill_array arguments
    fill_array_args_t args = { .scores = scores, .current_index = 0, .array_size = num_results };

    // Step 4: Populate the array using counters_iterate with fill_array
    counters_iterate(results, &args, fill_array);

    // Step 5: Sort scores in descending order (same as in the previous version)
    for (int i = 0; i < args.current_index - 1; i++) {
        int max = i;
        for (int j = i + 1; j < args.current_index; j++) {
            if (scores[j].score > scores[max].score) {
                max = j;
            }
        }
        if (max != i) {
            doc_score_t temp = scores[i];
            scores[i] = scores[max];
            scores[max] = temp;
        }
    }

    // Step 6: Print the results in rank order
    printf("Matches in rank order:\n");
    for (int i = 0; i < args.current_index; i++) {
        // Construct the file path for each document
        char filename[256];
        sprintf(filename, "%s/%d", pageDirectory, scores[i].docID);
        FILE *fp = fopen(filename, "r");
        if (fp != NULL) {
            char url[500];
            if (fgets(url, sizeof(url), fp) != NULL) {
                // Strip newline if present
                url[strcspn(url, "\n")] = 0;
                printf("score: %3d doc: %3d: %s\n", scores[i].score, scores[i].docID, url);
            }
            fclose(fp);
        } else {
            fprintf(stderr, "Warning: Could not open file %s\n", filename);
        }
    }

    // Step 7: Free allocated memory
    free(scores);
}



/*
 * Helper functions for counters operations
 */
static void count_nonzero(void *arg, const int key, const int count) {
    int *nitems = arg;
    if (count > 0) {
        (*nitems)++;
    }
}


void fill_array(void *arg, const int docID, const int count) {
    // Unpack the arguments: `arg` is expected to be a pointer to fill_array_args_t
    fill_array_args_t *args = arg;
    doc_score_t *tracker = args->scores;
    int *current_index = &args->current_index;
    int array_size = args->array_size;

    // Ensure count is positive and we are within array bounds
    if (count > 0 && *current_index < array_size) {
        tracker[*current_index].docID = docID;
        tracker[*current_index].score = count;

        (*current_index)++;  // Increment index
    } else if (*current_index >= array_size) {
        fprintf(stderr, "Error: Index out of bounds in fill_array.\n");
    }
}



static void intersect_counters(counters_t *c1, counters_t *c2) {
    if (c1 == NULL || c2 == NULL) return;
    // For each item in c1, update its count to min(count1, count2)
    counters_iterate(c1, c2, intersect_helper);
}

static void intersect_helper(void *arg, const int key, const int count) {
    counters_t *c2 = arg;
    int count2 = counters_get(c2, key);
    if (count2 < count) {
        counters_set(c2, key, count2);
    }
}

static void union_counters(counters_t *c1, counters_t *c2) {
    if (c1 == NULL || c2 == NULL) return;
    // For each item in c2, add its count to c1
    counters_iterate(c2, c1, union_helper);
}

static void union_helper(void *arg, const int key, const int count) {
    counters_t *c1 = arg;
    int count1 = counters_get(c1, key);
    counters_set(c1, key, count1 + count);
}
