#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRAYLIST_INITIAL_CAPACITY 16

typedef struct arrayList {
    int size, capacity; // size = number of element, capacity = real size
    char** tab;
} arrayList;

void arrayList_init(arrayList*);
short arrayList_add(arrayList*, char*);
short arrayList_removeIndex(arrayList*, int);
short arrayList_remove(arrayList*, char*);
short arrayList_set(arrayList*, int, char*);
int arrayList_size(arrayList*);
int arrayList_index(arrayList*, char*);
short arrayList_contains(arrayList*, char*);
