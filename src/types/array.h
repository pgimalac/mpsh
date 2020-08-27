#ifndef ARRAY
#define ARRAY

typedef struct array_t {
    int size, capacity;
    char **tab;
} array_t;

/**
 * Allocate a new array and return a pointer to this array
 */
array_t *array_init();

/**
 * Add element at the end of array
 */
short array_add(array_t *, char *);

/**
 * remove element at the given index of the array
 */
short array_remove_index(array_t *, int, short);

/**
 * remove the given element (the first only) of the array
 */
short array_remove(array_t *, char *, short);

/**
 * change value of the given index in the element
 */
short array_set(array_t *, int, char *, short);

/**
 * Look for the given element in the list
 * return his index if found, return -1 otherwise
 */
int array_index(array_t *, char *);

/**
 * Return 1 if contains the element 0 otherwise
 */
short array_contains(array_t *, char *);

/**
 * free pointer to array and all memory used by this one
 */
void array_destroy(array_t *, short);

/**
 * Return the array and reallocate to have the exact size
 * Free the given array_t (if the realloc succeded) because
 * assumed useless after conversion
 */
char **array_to_tab(array_t *);

#endif
