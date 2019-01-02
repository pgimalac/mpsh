#ifndef H_LIST
#define H_LIST

#include <unistd.h>

typedef struct list_t {
    void* val;
    struct list_t* next;
} list_t;

/**
 * Create a new linked list whose first element is given by the arguments
 */
list_t* list_init(void*, list_t*);

/**
 * Add element at the head of the list
 * Return if it succeeded
 */
short list_add(list_t**, void*);

/**
 * Remove the given index of the list
 * Return if it succeeded
 */
short list_remove(list_t**, int);

short list_filter(list_t **l, int(*pred)(void*));

/**
 * Change the value of the given index of the list
 * Return if it succeeded
 */
short list_set(list_t*, int, void*);

/**
 * Return the size of the list
 */
int list_size(list_t*);

/**
 * Free all memory used by the list
 */
void list_destroy(list_t*, short free);

/**
 * Removes the first element of the list and returns it
 */
void *list_pop(list_t**);

/**
 * Transform the linked list into a NULL-terminated array
 * Free all nodes from the list
 */
void **list_to_tab(list_t *list, size_t sz);

void list_iter(list_t *list, void(*f)(void*));

#endif
