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

#include <stdbool.h>
#include "webpage.h"

/* Initializes the page directory.
 * Returns true if the directory is successfully marked as a crawler directory.
 */
bool pagedir_init(const char* pageDirectory);

/* Saves the webpage to the specified directory with the given document ID.
 * Each page is saved with its URL, depth, and HTML content.
 * Returns true if the page was saved successfully.
 */
bool pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H
