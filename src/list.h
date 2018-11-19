#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct list_t {
    void* val;
    struct list_t* next;
} list_t;

list_t* list_init(void* v);
list_t* list_addFirst(list_t*, void*);
list_t* list_remove(list_t*, int);
void list_set(list_t*, int, void*);
int list_size(list_t*);
void list_destroy(list_t*);
