/*
 * crawler.c - CS50 Tiny Search Engine Crawler
 *
 * The Crawler starts from a seed URL and explores all reachable webpages
 * within the same domain, up to a given maximum depth. Each page is saved
 * to a designated directory, and each unique URL is only fetched once.
 *
 * The program fetches each page, extracts URLs, and continues crawling
 * new pages until the maximum depth is reached or all pages are visited.
 *
 * Usage:
 *   ./crawler seedURL pageDirectory maxDepth
 *     seedURL       - The URL from where crawling begins
 *     pageDirectory - The directory where pages are saved
 *     maxDepth      - The maximum depth to crawl (0-10)
 *
 * Dependencies:
 *   Requires libcs50 for data structures such as hashtable and bag.
 *   Requires page-specific modules like webpage and pagedir.
 *
 * Author: Chikwanda Chisha
 * Date: 20 October, 2024. (Fall)
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>  // for sleep()
#include "webpage.h"
#include "hashtable.h"
#include "bag.h"
#include "pagedir.h"
#include "crawler.h"
#include "mem.h"

// Function prototypes
void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
void delete_url(void* item);  // Helper function for cleaning up hashtable items

/**************** main() ****************/
/* Entry point for the crawler program. Parses arguments, initializes the
 * crawler, and begins crawling from the seedURL to the specified depth.
 */
int main(const int argc, char* argv[]) {
    char* seedURL = NULL;
    char* pageDirectory = NULL;
    int maxDepth = 0;

    // Parse and validate command-line arguments
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
    
    // Begin the crawling process from the seed URL
    crawl(seedURL, pageDirectory, maxDepth);
    
    // Free dynamically allocated memory for the normalized seedURL
    // free(seedURL);

    return 0;
}

/**************** parseArgs() ****************/
/* Parses command-line arguments, validates each argument, normalizes and
 * checks the seedURL, and ensures the pageDirectory exists and is writable.
 * If any argument is invalid, the program exits with an error.
 */
void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth) {
    // Verify argument count
    if (argc != 4) {
        fprintf(stderr, "usage: %s: seedURL pageDirectory maxDepth\n", argv[0]);
        exit(1);
    }

    // Normalize the seed URL
    char* normalized = normalizeURL(argv[1]);
    if (normalized == NULL) {
        fprintf(stderr, "error: cannot normalize seedURL\n");
        exit(1);
    }

    // Check if the URL is internal
    if (!isInternalURL(normalized)) {
        fprintf(stderr, "error: seedURL must be internal to CS50 server\n");
        free(normalized);  // Free memory before exit
        exit(1);
    }
    *seedURL = normalized;

    // Set pageDirectory and parse maxDepth
    *pageDirectory = argv[2];
    *maxDepth = atoi(argv[3]);

    // Validate maxDepth
    if (*maxDepth < 0 || *maxDepth > 10) {
        fprintf(stderr, "error: maxDepth must be an integer from 0 to 10\n");
        free(normalized);  // Free memory before exit
        exit(1);
    }

    // Check if pageDirectory is valid and writable
    if (!pagedir_init(*pageDirectory)) {
        fprintf(stderr, "error: pageDirectory must exist and be writable\n");
        free(normalized);  // Free memory before exit
        exit(1);
    }
}


/**************** crawl() ****************/
/* Initiates the crawling process from the seed URL up to maxDepth.
 * Pages are stored in the provided pageDirectory, each with a unique ID.
 * Only internal URLs are processed, and duplicate URLs are ignored.
 */


void crawl(char* seedURL, char* pageDirectory, const int maxDepth) {
    // Create a hashtable to track seen URLs and a bag for URLs to crawl
    hashtable_t* pagesSeen = hashtable_new(200);
    bag_t* pagesToCrawl = bag_new();

    // Check for successful data structure initialization
    if (pagesSeen == NULL || pagesToCrawl == NULL) {
        fprintf(stderr, "error: failed to initialize data structures\n");
        hashtable_delete(pagesSeen, delete_url);
        bag_delete(pagesToCrawl, NULL);
        exit(1);
    }

    // Duplicate seedURL for use as the key in hashtable
    char* duplicateSeedURL = strdup(seedURL);
    if (duplicateSeedURL == NULL) {
        fprintf(stderr, "error: failed to duplicate seedURL\n");
        hashtable_delete(pagesSeen, delete_url);
        bag_delete(pagesToCrawl, NULL);
        exit(1);
    }

    // Insert the duplicated seed URL into the hashtable to mark it as seen
    hashtable_insert(pagesSeen, duplicateSeedURL, "");  // Use empty string as value

    // Create a webpage structure for the seed URL, with depth 0
    webpage_t* webpage = webpage_new(seedURL, 0, NULL);
    if (webpage == NULL) {
        fprintf(stderr, "error: failed to create webpage\n");
        free(duplicateSeedURL);
        hashtable_delete(pagesSeen, delete_url);
        bag_delete(pagesToCrawl, NULL);
        exit(1);
    }

    // Add the seed webpage to the bag for processing
    bag_insert(pagesToCrawl, webpage);
    int docID = 1;  // Initialize document ID counter
    webpage_t* current;

    // Process each webpage in the bag until there are no more URLs to crawl
    while ((current = bag_extract(pagesToCrawl)) != NULL) {
        // Fetch HTML content of the current page
        if (webpage_fetch(current)) {
            // Save the fetched page to the pageDirectory with a unique docID
            pagedir_save(current, pageDirectory, docID++);

            // If within maxDepth, scan the page for URLs to add to the crawl
            if (webpage_getDepth(current) < maxDepth) {
                pageScan(current, pagesToCrawl, pagesSeen);
            }
        }
        // Free the current webpage after processing
        webpage_delete(current);
    }

    // Clean up: delete the hashtable and bag
    hashtable_delete(pagesSeen, delete_url);
    bag_delete(pagesToCrawl, NULL);
}



/**************** pageScan() ****************/
/* Scans a webpage for any URLs, ignoring non-internal URLs and duplicates.
 * If a valid new URL is found, it is added to both the hashtable and bag.
 */
void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen) {
    char* nextURL;
    int pos = 0;

    while ((nextURL = webpage_getNextURL(page, &pos)) != NULL) {
        char* normalizedURL = normalizeURL(nextURL);
        free(nextURL);  // Free the original URL returned by webpage_getNextURL

        if (normalizedURL == NULL || !isInternalURL(normalizedURL)) {
            free(normalizedURL);  // Free if invalid or not internal
            continue;
        }

        // Duplicate the URL to ensure it's dynamically allocated
        char* duplicateURL = strdup(normalizedURL);
        free(normalizedURL);  // Free the normalized URL after duplication

        if (hashtable_insert(pagesSeen, duplicateURL, "")) {
            // Only insert into pagesSeen if it was successfully added
            webpage_t* newPage = webpage_new(duplicateURL, webpage_getDepth(page) + 1, NULL);
            bag_insert(pagesToCrawl, newPage);
        } else {
            free(duplicateURL);  // Free duplicate if already in pagesSeen
        }
    }
}



/**************** delete_url() ****************/
/* Helper function to free URL strings stored in the hashtable.
 * This function is passed to hashtable_delete to ensure URLs are freed.
 */
void delete_url(void* item) {
    if (item != NULL && item != "" && strcmp(item, "") != 0) { // Only free if not the empty string
        free(item);  
    }
}
