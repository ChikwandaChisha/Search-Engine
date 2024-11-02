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
#include <ctype.h>
#include <dirent.h>
#include "../libcs50/webpage.h"
#include "pagedir.h"
#include "../libcs50/file.h"  // Updated to use file.h for file utilities
#include "../libcs50/mem.h"

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

bool pagedir_validate(const char *pageDirectory) {
    // Example: check if a directory has a .crawler file to validate it
    char path[1024];
    snprintf(path, sizeof(path), "%s/.crawler", pageDirectory);
    FILE *file = fopen(path, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

/**************** pagedir_load ****************/
/* Loads a webpage from the specified file path.
 * Reads the URL from the first line, depth from the second line, and the rest as HTML content.
 */
webpage_t *pagedir_load(const char *filepath) {
    // Open the file in read mode
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        // Return NULL if the file cannot be opened
        return NULL;
    }

    // Read the URL from the first line using file_readLine
    char *url = file_readLine(file);  // Updated from freadlinep to file_readLine
    if (url == NULL) {
        // If URL cannot be read, close the file and return NULL
        fclose(file);
        return NULL;
    }

    // Read the depth from the next line
    int depth;
    if (fscanf(file, "%d\n", &depth) != 1) {
        // If depth is not read correctly, free URL, close file, and return NULL
        free(url);
        fclose(file);
        return NULL;
    }

    // Read the HTML content from the rest of the file using file_readFile
    char *html = file_readFile(file);  // Updated from freadfilep to file_readFile
    fclose(file);

    if (html == NULL) {
        // If HTML cannot be read, free URL and return NULL
        free(url);
        return NULL;
    }

    // Create a new webpage with the URL, depth, and HTML content
    webpage_t *page = webpage_new(url, depth, html);

    // Do not free `url` and `html` here, as `webpage_new` does not copy them
    // Return the created webpage structure
    return page;
}
