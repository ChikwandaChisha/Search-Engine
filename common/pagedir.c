/* pagedir.c - module to handle page directory operations for CS50 TSE Crawler
 *
 * This module includes functions to:
 * 1. Initialize the page directory and mark it as a crawler directory.
 * 2. Save each fetched page in the specified directory with a unique document ID.
 *
 * Author: Chikwanda Chisha
 * Date: 18 October, 2024.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "webpage.h"
#include "pagedir.h"

/**************** pagedir_init ****************/
/* Initialize the page directory.
 * If successful, creates a .crawler file within the directory to mark it as
 * a valid crawler directory.
 */
bool pagedir_init(const char* pageDirectory) {
    // Construct the path for the .crawler file
    char path[strlen(pageDirectory) + 10];
    sprintf(path, "%s/.crawler", pageDirectory);

    // Try to create the file
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        return false; // Could not open .crawler file for writing
    }
    fclose(file);
    return true;
}

/**************** pagedir_save ****************/
/* Save the webpage to the directory with a unique document ID.
 * Each saved file contains:
 * 1. The URL on the first line
 * 2. The depth of the page on the second line
 * 3. The HTML content starting on the third line
 */
bool pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID) {
    // Construct the path for the file
    char path[strlen(pageDirectory) + 20];
    sprintf(path, "%s/%d", pageDirectory, docID);

    // Open the file for writing
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        return false;
    }

    // Write URL, depth, and HTML content
    fprintf(file, "%s\n", webpage_getURL(page));
    fprintf(file, "%d\n", webpage_getDepth(page));
    fprintf(file, "%s\n", webpage_getHTML(page));

    fclose(file); // Close the file
    return true;
}
