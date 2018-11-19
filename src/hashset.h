#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "list.h"

typedef struct hashset_t {
    int size, capacity; // size = number of element, capacity = real size
    list_t** tab;
} hashset_t;

hashset_t* hashset_init();
short hashset_add(hashset_t*, char*);
short hashset_remove(hashset_t*, char*);
short hashset_contains(hashset_t*, char*);
void hashset_destroy(hashset_t*);
