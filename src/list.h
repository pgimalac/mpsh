#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct list_t {
    void* val;
    struct list_t* next;
} list_t;

/**
 * Create a new linked list with one element
 */
list_t* list_init(void* v);

/**
 * Add element at the head of the list
 */
list_t* list_add(list_t*, void*);

/**
 * Remove the given index of the list
 */
list_t* list_remove(list_t*, int);

/**
 * Change the value of the given index of the list
 */
void list_set(list_t*, int, void*);

/**
 * Return the size of the list
 */
int list_size(list_t*);

/**
 * Free all memory used by the list
 */
void list_destroy(list_t*);
