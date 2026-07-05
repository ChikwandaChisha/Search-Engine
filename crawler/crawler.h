/* crawler.h - header file for Tiny Search Engine Crawler
 *
 * This file contains function declarations, constants, and any necessary
 * types needed by the Crawler module for TSE. The Crawler starts
 * from a seed URL and explores all reachable pages up to a specified
 * depth, saving each page's content in a designated directory.
 *
 * Author: Chikwanda Chisha
 * Date: 18 October, 2024.
 */


#ifndef __CRAWLER_H
#define __CRAWLER_H

#include "webpage.h"

/* Maximum depth for crawling */
#define MAX_DEPTH 10

/* Parse command-line arguments, validate input, normalize seed URL */
void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);

/* Start crawling from seedURL to maxDepth, saving pages to pageDirectory */
void crawl(char* seedURL, char* pageDirectory, const int maxDepth);

/* Scan webpage for URLs, adding new, internal ones to pagesToCrawl and pagesSeen */
void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

/* Free function for URLs in the hashtable */
void delete_url(void* item);

#endif // __CRAWLER_H
