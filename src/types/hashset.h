#ifndef H_HASHSET
#define H_HASHSET

#include "list.h"

typedef struct hashset_t {
    int size, capacity;
    list_t **tab;
} hashset_t;

/**
 * Allocate a new hashset
 */
hashset_t *hashset_init();

/**
 * Add the given element in the given set
 */
short hashset_add(hashset_t *, char *);

/**
 * Remove the given element of the set
 */
short hashset_remove(hashset_t *, char *);

/**
 * return 1 if the given element is in the set, 0 otherwise
 */
short hashset_contains(hashset_t *, char *);

/**
 * free all memory used by the set
 */
void hashset_destroy(hashset_t *, short);

#endif
