/* pagedir.h - header file for pagedir.c module
 *
 * This module provides functions to initialize a page directory
 * and save fetched webpages.
 * 
 * Author: Chikwanda Chisha
 * Date: 18 October 2024.
 */

#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdio.h>
#include <stdlib.h>
#include "../libcs50/webpage.h"

/* Initializes the page directory.
 * Returns true if the directory is successfully marked as a crawler directory.
 */
bool pagedir_init(const char* pageDirectory);


/* Saves the webpage to the specified directory with the given document ID.
 * Each page is saved with its URL, depth, and HTML content.
 * Returns true if the page was saved successfully.
 */
bool pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);


/* Validates if the specified directory was produced by the crawler.
 * Checks if the directory contains a ".crawler" file to verify.
 * Returns true if the directory is valid, false otherwise.
 */
bool pagedir_validate(const char *pageDirectory);


/* Loads a webpage from the specified file path.
 * Reads the URL, depth, and HTML content from the file to create a webpage.
 * Returns a pointer to the loaded webpage, or NULL if loading fails.
 */
webpage_t *pagedir_load(const char *filepath);


#endif // __PAGEDIR_H
