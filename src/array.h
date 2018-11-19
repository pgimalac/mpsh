#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct array_t {
    int size, capacity; // size = number of element, capacity = real size
    char** tab;
} array_t;

array_t* array_init();
short array_add(array_t*, char*);
short array_removeIndex(array_t*, int);
short array_remove(array_t*, char*);
short array_set(array_t*, int, char*);
int array_index(array_t*, char*);
short array_contains(array_t*, char*);
void array_destroy(array_t*);
