#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRAY_INITIAL_CAPACITY 16

typedef struct array {
    int size, capacity; // size = number of element, capacity = real size
    char** tab;
} array;

array* array_init();
short array_add(array*, char*);
short array_removeIndex(array*, int);
short array_remove(array*, char*);
short array_set(array*, int, char*);
int array_size(array*);
int array_index(array*, char*);
short array_contains(array*, char*);
