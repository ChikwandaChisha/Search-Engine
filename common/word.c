/*
 * word.c - Utility functions for word processing in the Tiny Search Engine
 *
 * Provides helper functions for word normalization, ensuring that all words
 * are indexed in lowercase, making the index case-insensitive.
 *
 * Author: Chikwanda Chisha
 * Date: October 26, 2024
 */

#include <ctype.h>
#include "word.h"

/**************** normalizeWord() ****************/
/* Converts all characters in the given word to lowercase.
 * word: a non-null, valid string to be normalized.
 * Modifies the word in place to ensure consistent casing for indexing.
 */
void normalizeWord(char *word) {
    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = tolower(word[i]);
    }
}
