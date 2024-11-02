/* 
 * counters.c - CS50 'counters' module
 *
 * see counters.h for more information.
 *
 * Author: Chikwanda Chisha.
 * Date: 27 October, 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "counters.h"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct countersnode {
  int* key;
  int* counter;                 // pointer to data for this counter
  struct countersnode *next;       // link to next node
} countersnode_t;

/**************** global types ****************/
typedef struct counters {
  struct countersnode *head;       // head of the list of counters in counters
} counters_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static countersnode_t* countersnode_new(const int* key);
//static uniquecounters()

/**************** counters_new() ****************/
/* see counters.h for description */
counters_t*
counters_new(void)
{
  counters_t* counters = mem_malloc(sizeof(counters_t));

  if (counters == NULL) {
    return NULL;              // error allocating counters
  } else {
    // initialize contents of counters structure
    counters->head = NULL;
    return counters;
  }
}

/**************** counters_add() ****************/
/* see counters.h for description */
int
counters_add(counters_t* ctrs, const int key)
{ 
  if (ctrs == NULL || key < 0){
    return 0;
  }
  if (ctrs != NULL) {
     for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
      if (*(node->key) ==  key){ // if key exists in ctrs
        *(node->counter) +=1;
        return *(node->counter);
      }
     }
    // allocate a new node to be added to the list
    countersnode_t* new = countersnode_new(&key);
    if (new != NULL) {
      // add it to the head of the list
      
      new->next = ctrs->head;
      ctrs->head = new;
    }
    return *(new->counter);
  }
  return 0;

#ifdef MEMTEST
  mem_report(stdout, "After counters_add");
#endif
}


/**************** countersnode_new ****************/
/* Allocate and initialize a countersnode */
static countersnode_t*  // not visible outside this file
countersnode_new(const int* key)
{
  countersnode_t* node = mem_malloc(sizeof(countersnode_t));
  node->counter = mem_malloc(sizeof(int));
  node->key = mem_malloc(sizeof(int));
      
  
  if (node == NULL) {
    // error allocating memory for node; return error
    return NULL;
  } else {
    if (node->counter == NULL) {
    // error allocating memory for counter; return error
    mem_free(node);
    return NULL;
    }
    //node->key = key;
    memcpy(node->key, key, sizeof(int));
    *(node->counter) = 1;
    node->next = NULL;
    return node;
  }
}

/**************** counters_get() ****************/
/* see counters.h for description */
int
counters_get(counters_t* ctrs, const int key)
{
  if (ctrs == NULL || key < 0) {
    return 0;              // bad counters
  } else if (ctrs->head == NULL) {
    return 0;              // ctrs is empty
  } else {
    for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) { // loop through the ctrs
        if (*(node->key) == key){
          return *(node->counter); // desired counter
        }
    }
  }
  return 0;
}

/**************** counters_set() ****************/
/* see counters.h for description */
bool
counters_set(counters_t* ctrs, const int key, const int count)
{
  if (ctrs == NULL || key < 0 || count < 0) {
    return false; 
  }
  if (ctrs != NULL) {
     for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
      if (*(node->key) == key){ // if key found in ctrs
        *(node->counter) = count;
        return true;
      }
     }
    // allocate a new node to be added to the list
    countersnode_t* new = countersnode_new(&key);
    if (new != NULL) {
      // add it to the head of the list
      *(new->counter) = count; // create a counter for it and initialize to the given value. 
      new->next = ctrs->head;
      ctrs->head = new;
      return true;
    }
  }
  return false;

#ifdef MEMTEST
  mem_report(stdout, "After counters_set");
#endif
}

/**************** counters_print() ****************/
/* see counters.h for description */
void
counters_print(counters_t* ctrs, FILE* fp)
{
  if (fp != NULL) {
    if (ctrs != NULL) {
      fputc('{', fp);
      for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
        // print this node
        fprintf(fp, "%p = %d", (void*)node->key, *(node->counter));
        // Check if current element is the last element
        if(node->next != NULL) {
            fputc(',', fp);
        } 
      }
      fprintf(fp, "%s", "}\n");
    } else {
      fputs("(null)", fp);
    }
  }
}

/**************** counters_iterate() ****************/
/* see counters.h for description */
void
counters_iterate(counters_t* ctrs, void* arg, void (*itemfunc)(void* arg, const int key, const int count) )
{
  if (ctrs != NULL && itemfunc != NULL) {
    // call itemfunc with arg, on each counter
    for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
      (*itemfunc)(arg, *(node->key), *(node->counter)); 
    }
  }
}

/**************** counters_delete() ****************/
/* see counters.h for description */
void 
counters_delete(counters_t* ctrs)
{
  if (ctrs != NULL) {
    for (countersnode_t* node = ctrs->head; node != NULL; ) {
     
      countersnode_t* next = node->next;     // remember what comes next
      mem_free(node->key);              // free key string for each counter
      mem_free(node->counter);
      mem_free(node);                   // free the node
      node = next;                      // and move on to next
    }

    mem_free(ctrs);
  }

#ifdef MEMTEST
  mem_report(stdout, "End of counters_delete");
#endif
}