#ifndef HASHMAP
#define HASHMAP

#include "list.h"

typedef struct hashmap {
    int size, capacity;
    list_t **tab;
} hashmap_t;

/**
 * Allocate a new hashset
 */
hashmap_t *hashmap_init();

/**
 * Add the given element in the given set
 * Change the value if the element already exists
 */
short hashmap_add(hashmap_t *, char *, char *, short);

/**
 * Return the value for a given key
 * return NULL if the key does not exists
 */
char *hashmap_get(hashmap_t *, char *);

/**
 * Remove the given element of the set
 */
short hashmap_remove(hashmap_t *, char *, short);

/**
 * return 1 if the given element is in the set, 0 otherwise
 */
short hashmap_contains(hashmap_t *, char *);

/**
 * free all memory used by the set
 */
void hashmap_destroy(hashmap_t *, short);

/**
 * print all key value couple store in the map
 */
void hashmap_print(hashmap_t *, int);

/**
 * iterate all the key/value of the map in an undefined order
 */
void hashmap_iterate(hashmap_t *, int, void (*)(int, char *, char *));

#endif
