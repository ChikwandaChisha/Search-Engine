/*
 * word.h - Header file for word processing utility in the Tiny Search Engine
 *
 * Declares the normalizeWord function, which converts words to lowercase
 * for consistent indexing.
 *
 * Author: Chikwanda Chisha
 * Date: 27 October, 2024
 */

#ifndef __WORD_H
#define __WORD_H

/**************** normalizeWord() ****************/
/* Converts all characters in the given word to lowercase.
 * word: a non-null, valid string to be normalized.
 * Modifies the word in place to ensure consistent casing for indexing.
 */
void normalizeWord(char *word);

#endif // __WORD_H
