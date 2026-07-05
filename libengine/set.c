/* 
 * Author: Chikwanda Chisha
 * Date: Oct 10, 2024
 * counters.c - Lab 3
 * 
 * This module implements a set of counters, each identified by a unique non-negative integer key.
 * The set is implemented as a linked list of nodes, each storing a key-counter pair.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"

/************ Internal Data Structures ************/
// Define the structure of a set node (key-item pair)
typedef struct setnode {
    char *key;            // key is a string
    void *item;           // pointer to the item associated with the key
    struct setnode *next; // next node in the list
} setnode_t;

// Define the structure of the set itself
struct set {
    struct setnode *head; // pointer to the first node in the list
};

/************ Local Function Prototypes ************/
static setnode_t* setnode_new(const char *key, void *item); // function to create a new set node

/************ Functions ************/

/**************** set_new ****************/
set_t* set_new(void) {
    set_t *set = malloc(sizeof(set_t));
    if (set == NULL) {
        return NULL; // return NULL if memory allocation fails
    } else {
        set->head = NULL; // initialize the set as empty
        return set;
    }
}

/**************** setnode_new ****************/
static setnode_t* setnode_new(const char *key, void *item) {
    setnode_t *node = malloc(sizeof(setnode_t));
    if (node == NULL) {
        return NULL; // return NULL if memory allocation fails
    }
    
    // Allocate memory for the key string and copy the key
    node->key = malloc(strlen(key) + 1);
    if (node->key == NULL) {
        free(node);
        return NULL;
    }
    strcpy(node->key, key);
    
    node->item = item; // set the item pointer
    node->next = NULL; // initialize the next pointer as NULL
    return node;
}

/**************** set_insert ****************/
bool set_insert(set_t* set, const char* key, void* item) {
    if (set == NULL || key == NULL) {
        return false; // error if set or key is NULL
    }

    // Check if the key already exists in the set
    for (setnode_t *node = set->head; node != NULL; node = node->next) {
        if (strcmp(node->key, key) == 0) {
            return false; // key already exists
        }
    }

    // Create a new set node
    setnode_t *newnode = setnode_new(key, item);
    if (newnode == NULL) {
        return false; // return false if memory allocation failed
    }

    // Insert the new node at the head of the list
    newnode->next = set->head;
    set->head = newnode;
    return true;
}

/**************** set_find ****************/
void* set_find(set_t* set, const char* key) {
    if (set == NULL || key == NULL) {
        return NULL; // error if set or key is NULL
    }

    // Iterate through the set to find the key
    for (setnode_t *node = set->head; node != NULL; node = node->next) {
        if (strcmp(node->key, key) == 0) {
            return node->item; // return the item if key is found
        }
    }
    return NULL; // return NULL if key is not found
}

/**************** set_print ****************/
void set_print(set_t* set, FILE* fp, 
               void (*itemprint)(FILE* fp, const char* key, void* item)) {
    if (fp == NULL) {
        return; // do nothing if file pointer is NULL
    }

    if (set == NULL) {
        fprintf(fp, "(null)\n");
        return;
    }

    fprintf(fp, "{"); // start the set output
    for (setnode_t *node = set->head; node != NULL; node = node->next) {
        // print each key-item pair using the provided itemprint function
        if (itemprint != NULL) {
            (*itemprint)(fp, node->key, node->item);
        }
        if (node->next != NULL) {
            fprintf(fp, ", "); // print a comma if there are more elements
        }
    }
    fprintf(fp, "}\n"); // end the set output
}

/**************** set_iterate ****************/
void set_iterate(set_t* set, void* arg, 
                 void (*itemfunc)(void* arg, const char* key, void* item)) {
    if (set == NULL || itemfunc == NULL) {
        return; // do nothing if set or function pointer is NULL
    }

    // Iterate over all elements and call itemfunc for each one
    for (setnode_t *node = set->head; node != NULL; node = node->next) {
        (*itemfunc)(arg, node->key, node->item); // call the function on each item
    }
}

/**************** set_delete ****************/
void set_delete(set_t* set, void (*itemdelete)(void* item)) {
    if (set == NULL) {
        return; // do nothing if set is NULL
    }

    // Iterate through the list and delete each node
    setnode_t *node = set->head;
    while (node != NULL) {
        setnode_t *next = node->next; // save the next node

        // If itemdelete function is provided, call it on the item
        if (itemdelete != NULL) {
            (*itemdelete)(node->item);
        }

        free(node->key);  // free the key string
        free(node);       // free the node itself
        node = next;      // move to the next node
    }

    // Finally, free the set structure itself
    free(set);
}
