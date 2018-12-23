#include "array.h"

#define ARRAY_INITIAL_CAPACITY 16

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

array_t *array_init(){
    array_t* a = malloc(sizeof(array_t));
    if (a) {
        a->size = 0;
        a->capacity = ARRAY_INITIAL_CAPACITY;

        if (!(a->tab = (char**)malloc(sizeof(char*) * a->capacity))){
            free(a);
            a = NULL;
        }
    }
    return a;
}

static short array_resize(array_t* a, int capacity){
    if (a != NULL){
        char **tab = realloc(a->tab, sizeof(char *) * capacity);
        if (tab) {
            a->tab = tab;
            a->capacity = capacity;
            return 1;
        }
    }
    return 0;
}

short array_add(array_t* a, char* s){
    if (a != NULL && (a->size != a->capacity || array_resize(a, 2 * a->size))){
        a->tab[a->size++] = s;
        return 1;
    }
    return 0;
}

short array_remove_index(array_t* a, int i, short f){
    if (a == NULL || i < 0 || i >= a->size)
        return 0;

    a->size--;

    if (f)
        free(a->tab[i]);
    do
        a->tab[i] = a->tab[i + 1];
    while (++i < a->size);

    if (a->capacity >= 4 * a->size && a->capacity > ARRAY_INITIAL_CAPACITY)
        array_resize(a, a->capacity / 2);

    return 1;
}

short array_remove(array_t* a, char* st, short s){
    if (a != NULL)
        for (int i = 0; i < a->size; i++)
            if (strcmp(a->tab[i], st) == 0)
                return array_remove_index(a, i, s);
    return 0;
}

short array_set(array_t* a, int i, char* s, short f){
    if (a == NULL || i < 0 || i >= a->size)
        return 0;

    if (f)
       free(a->tab[i]);

    a->tab[i] = s;
    return 1;
}

int array_index(array_t* a, char* s){
    if (a != NULL)
        for (int i = 0; i < a->size; i++)
            if (strcmp(a->tab[i], s) == 0)
                return i;
    return -1;
}

short array_contains(array_t* a, char* s){
    return array_index(a, s) != -1;
}

void array_destroy(array_t* a, short f){
    if (a != NULL){
        if (f)
            for (int i = 0; i < a->size; i++)
                free(a->tab[i]);

        free(a->tab);
        free(a);
    }
}

char **array_to_tab (array_t *array) {
    char **ret = realloc(array->tab, sizeof(char *) * array->size);
    free(array);
    return ret;
}
