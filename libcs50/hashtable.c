/*
 * Author: Chikwanda Chisha
 * Date: 11 October, 2024
 * hashtable.c - CS50 Lab 3
 *
 * This module implements a hashtable that stores (key, item) pairs.
 * 
 * Each key is a string, and each item is a generic pointer (void*).
 * 
 * The hashtable is implemented as an array of slots, where each slot is
 * a set (from set.c) that handles key-item pairs. 
 * 
 * The hash function uses a prime number multiplier (31) and modulus to
 * distribute keys across the slots. Collisions are handled via chaining
 * (each slot is a set).
 *
 * Memory management: The caller is responsible for managing the memory of
 * items, and the hashtable module manages the memory for keys (copies them).
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "set.h"  // Using the set module to store key-item pairs in each slot

/**************** local types ****************/

// The hashtable is an array of set pointers (each set stores key-item pairs)
typedef struct hashtable {
    int num_slots;      // Number of slots in the hashtable (array size)
    set_t** slots;      // Array of pointers to sets (one set per slot)
} hashtable_t;

/**************** local functions ****************/

/* Hash function that uses modulus based on the method from the PDF */
static int hash_function(const char* key, int num_slots) {
    if (key == NULL || num_slots <= 0) {
        return -1;  // Invalid key or number of slots
    }

    unsigned long hash = 0;
    int prime = 31;  // Prime number used for hashing, commonly chosen for good distribution

    // Compute the hash value using a multiplicative approach
    for (const char* p = key; *p != '\0'; p++) {
        hash = (hash * prime + *p) % num_slots;  // hash is compressed by num_slots
    }

    return hash % num_slots;  // Final compression using modulus
}

/**************** hashtable_new ****************/
/*
 * Create a new hashtable with a given number of slots.
 * Each slot is initialized with an empty set.
 */
hashtable_t* hashtable_new(const int num_slots) {
    if (num_slots <= 0) {
        return NULL;  // Invalid number of slots
    }

    // Allocate memory for the hashtable structure
    hashtable_t* ht = malloc(sizeof(hashtable_t));
    if (ht == NULL) {
        return NULL;  // Memory allocation failed
    }

    // Allocate memory for the array of set pointers (slots)
    ht->slots = malloc(num_slots * sizeof(set_t*));
    if (ht->slots == NULL) {
        free(ht);
        return NULL;  // Memory allocation failed
    }

    // Initialize each slot with an empty set
    for (int i = 0; i < num_slots; i++) {
        ht->slots[i] = set_new();  // Create a new empty set for each slot
    }

    ht->num_slots = num_slots;  // Store the number of slots
    return ht;  // Return the new hashtable
}

/**************** hashtable_insert ****************/
/*
 * Insert an item into the hashtable. The item is associated with a given key.
 * If the key already exists in the hashtable, insertion fails.
 * Returns true if the item was successfully inserted, false otherwise.
 */
bool hashtable_insert(hashtable_t* ht, const char* key, void* item) {
    if (ht == NULL || key == NULL || item == NULL) {
        return false;  // Invalid input
    }

    // Hash the key to find the correct slot (index)
    int index = hash_function(key, ht->num_slots);
    if (index < 0) {
        return false;  // Error in hashing
    }

    // Insert the key-item pair into the appropriate set in the slot
    return set_insert(ht->slots[index], key, item);
}

/**************** hashtable_find ****************/
/*
 * Find an item in the hashtable by its key.
 * Returns the item if found, or NULL if the key does not exist.
 */
void* hashtable_find(hashtable_t* ht, const char* key) {
    if (ht == NULL || key == NULL) {
        return NULL;  // Invalid input
    }

    // Hash the key to find the correct slot (index)
    int index = hash_function(key, ht->num_slots);
    if (index < 0) {
        return NULL;  // Error in hashing
    }

    // Find the item associated with the key in the appropriate set (slot)
    return set_find(ht->slots[index], key);
}

/**************** hashtable_print ****************/
/*
 * Print the contents of the hashtable.
 * Each key-item pair is printed using the provided itemprint function.
 */
void hashtable_print(hashtable_t* ht, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item)) {
    if (fp == NULL) {
        return;  // Invalid file pointer
    }

    if (ht == NULL) {
        fprintf(fp, "(null)\n"); // Print "(null)" if the hashtable is NULL and a new line
        return;
    }

    // Iterate over all slots in the hashtable
    for (int i = 0; i < ht->num_slots; i++) {
        fprintf(fp, "Slot %d: ", i);  // Print the slot number
        // Print the contents of the set in each slot
        set_print(ht->slots[i], fp, itemprint);
    }
}

/**************** hashtable_iterate ****************/
/*
 * Iterate through all key-item pairs in the hashtable.
 * Calls the itemfunc function on each key-item pair.
 */
void hashtable_iterate(hashtable_t* ht, void* arg, void (*itemfunc)(void* arg, const char* key, void* item)) {
    if (ht == NULL || itemfunc == NULL) {
        return;  // Invalid input
    }

    // Iterate over all slots in the hashtable
    for (int i = 0; i < ht->num_slots; i++) {
        // Iterate over each set in the hashtable and apply the itemfunc
        set_iterate(ht->slots[i], arg, itemfunc);
    }
}

/**************** hashtable_delete ****************/
/*
 * Delete the hashtable and all its contents.
 * Frees the memory for each key-item pair and the hashtable itself.
 */
void hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item)) {
    if (ht == NULL) {
        return;  // Invalid input
    }

    // Iterate over all slots and delete each set in the hashtable
    for (int i = 0; i < ht->num_slots; i++) {
        set_delete(ht->slots[i], itemdelete);  // Delete each set
    }

    // Free the array of set pointers and the hashtable structure itself respectively
    free(ht->slots);
    free(ht);
}
