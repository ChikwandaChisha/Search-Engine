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

struct intersect_helper_args {
    counters_t *c2;
    counters_t *result;
};


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
    
    // Process each character until end of string or newline
    for (int i = 0; query[i] != '\0' && query[i] != '\n'; i++) {
        if (isalpha(query[i])) {
            // Convert letters to lowercase and add to cleaned string
            cleaned[j++] = tolower(query[i]);
            lastWasSpace = false;
        }
        else if (isspace(query[i])) {
            // Normalize spaces: only one space between words
            if (!lastWasSpace) {
                cleaned[j++] = ' ';
                lastWasSpace = true;
            }
        }
        else {
            // Reject queries with non-letter, non-space characters
            fprintf(stderr, "Error: bad character '%c' in query.\n", query[i]);
            free(cleaned);              // Free allocated memory
            return NULL;                // Return NULL to indicate error
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

    // Process tokens until end of string
    while (token != NULL) {
        // Allocate memory for copy of current token
        tokens[i] = malloc(strlen(token) + 1);
        if (tokens[i] == NULL) {
            // If malloc fails, free all previously allocated tokens
            for (int j = 0; j < i; j++) {
                free(tokens[j]);
            }
            free(tokens);   // Free the array itself
            return NULL;    // Return NULL to indicate tokenization failed
        }
        strcpy(tokens[i], token);          // Copy token into array
        i++;                               // Move to next position
        token = strtok(NULL, " ");         // Get next token
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
 * Processes tokens and evaluates the query.
 */
counters_t *evaluate_query(char **tokens, index_t *index) {
    if (tokens == NULL || tokens[0] == NULL || index == NULL) return NULL;

    hashtable_t *ht = index_get_hashtable(index);
    if (ht == NULL) return NULL;

    counters_t *final_result = NULL;  // Holds the final result
    counters_t *current_and = NULL;   // Holds current AND sequence
    bool start_sequence = true;       // Flag for start of AND sequence

    for (int i = 0; tokens[i] != NULL; i++) {
        // Skip operator tokens
        if (strcmp(tokens[i], "or") == 0) {
            // Union the completed AND sequence into the final result
            if (current_and != NULL) {
                if (final_result == NULL) {
                    final_result = current_and;
                } else {
                    union_counters(final_result, current_and);
                    counters_delete(current_and);
                }
                current_and = NULL;  // Reset for next AND sequence
            }
            start_sequence = true;  // Ready for a new AND sequence
            continue;
        }

        // Get counters for current word
        counters_t *word_counters = hashtable_find(ht, tokens[i]);

        if (start_sequence) {
            // Initialize current AND sequence with this word's counters
            current_and = counters_new();
            if (word_counters != NULL) {
                counters_iterate(word_counters, current_and, union_helper);
            }
            start_sequence = false;  // Mark that sequence has started
        } else {
            // Intersect with the current word's counters for AND logic
            if (word_counters != NULL) {
                intersect_counters(current_and, word_counters);
            } else {
                // If word not found, set current AND result to empty
                counters_delete(current_and);
                current_and = counters_new();  // Empty result
            }
        }
    }

    // Process any remaining AND sequence at end of query
    if (current_and != NULL) {
        if (final_result == NULL) {
            final_result = current_and;      // First result becomes final result
        } else {
            union_counters(final_result, current_and);  // Merge with OR
            counters_delete(current_and);    // Free temporary AND sequence
        }
    }

    return final_result != NULL ? final_result : counters_new();
}



/* Comparator function for qsort to sort by score in descending order */ 
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

    // Count number of non-zero results
    int num_results = 0;
    counters_iterate(results, &num_results, count_nonzero);

    if (num_results == 0) {
        printf("No documents match.\n");
        return;
    }

    // Allocate memory for an array of doc_score_t to hold the scores
    doc_score_t *scores = calloc(num_results, sizeof(doc_score_t));
    if (scores == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in print_results.\n");
        return;
    }

    // Initialize the fill_array arguments
    fill_array_args_t args = { .scores = scores, .current_index = 0, .array_size = num_results };

    // Populate the array using counters_iterate with fill_array
    counters_iterate(results, &args, fill_array);

    // Sort scores in descending order (same as in the previous version)
    // Sort scores array in descending order using selection sort
    for (int i = 0; i < args.current_index - 1; i++) {
        int max = i;                    // Index of maximum score found so far
        // Find maximum score in unsorted portion
        for (int j = i + 1; j < args.current_index; j++) {
            if (scores[j].score > scores[max].score) {
                max = j;
            }
        }
        // Swap current position with maximum if needed
        if (max != i) {
            doc_score_t temp = scores[i];
            scores[i] = scores[max];
            scores[max] = temp;
        }
    }

    // Print ranked results
    printf("Matches in rank order:\n");
    for (int i = 0; i < args.current_index; i++) {
        // Get URL from document's crawler file
        char filename[256];
        sprintf(filename, "%s/%d", pageDirectory, scores[i].docID);
        FILE *fp = fopen(filename, "r");
        if (fp != NULL) {
            char url[500];
            // Read first line (URL) from crawler file
            if (fgets(url, sizeof(url), fp) != NULL) {
                url[strcspn(url, "\n")] = 0;  // Remove trailing newline
                // Print score, docID, and URL in formatted output
                printf("score: %3d doc: %3d: %s\n", scores[i].score, scores[i].docID, url);
            }
            fclose(fp);
        } else {
            fprintf(stderr, "Warning: Could not open file %s\n", filename);
        }
    }

    // Free allocated memory
    free(scores);
}


/*
 * Helper function for counters_iterate to count non-zero items.
 * Increments counter when document has positive count/score.
 */
static void count_nonzero(void *arg, const int key, const int count) {
    int *nitems = arg;              // Cast arg to counter
    if (count > 0) {
        (*nitems)++;                // Increment if document has positive score
    }
}


/**
 * Helper function for counters_iterate to fill score array.
 * Takes document ID and count pairs and stores them in array.
 */
void fill_array(void *arg, const int docID, const int count) {
    // Unpack arguments structure
    fill_array_args_t *args = arg;
    doc_score_t *tracker = args->scores;
    int *current_index = &args->current_index;
    int array_size = args->array_size;

    // Store docID/count pair if count > 0 and array has space
    if (count > 0 && *current_index < array_size) {
        tracker[*current_index].docID = docID;
        tracker[*current_index].score = count;
        (*current_index)++;  // Move to next array position
    } else if (*current_index >= array_size) {
        fprintf(stderr, "Error: Index out of bounds in fill_array.\n");
    }
}


/** Sets c1 to intersection with c2, keeping minimum scores. */
static void intersect_counters(counters_t *c1, counters_t *c2) {
    if (c1 == NULL || c2 == NULL) return;

    // Iterate over each key in c1 and adjust based on minimum counts in c2
    counters_iterate(c1, &(struct intersect_helper_args){c2, c1}, intersect_helper);
}


/*
 * Sets each doc's score to min of scores from c1,c2 or removes if not in c2.
 */
static void intersect_helper(void *arg, const int key, const int count) {
    struct intersect_helper_args *args = arg;     // Cast arg to helper struct
    int count2 = counters_get(args->c2, key);    // Get count from c2

    if (count2 > 0) {
        // Document exists in both sets: keep minimum score
        counters_set(args->result, key, count < count2 ? count : count2);
    } else {
        // Document not in c2: remove from result by setting count to 0
        counters_set(args->result, key, 0);
    }
}

/* Merge c1 and c2, summing their counts; result ends up in c1. */
static void union_counters(counters_t *c1, counters_t *c2) {
    if (c1 == NULL || c2 == NULL) return;
    // For each item in c2, add its count to c1
    counters_iterate(c2, c1, union_helper);
}


/* Helper for union_counters that adds count from c2 to corresponding count in c1. */
static void union_helper(void *arg, const int key, const int count) {
    counters_t *c1 = arg;               // Cast arg back to counters_t
    int count1 = counters_get(c1, key); // Get current count (0 if not in c1)
    counters_set(c1, key, count1 + count); // Set new combined count
}
