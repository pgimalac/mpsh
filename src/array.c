#include "array.h"

array* array_init(){
    array* a;
    if (!(a = malloc(sizeof(array)))){
        perror("Error creating an array.");
        exit(EXIT_FAILURE);
    }

    a->size = 0;
    a->capacity = ARRAY_INITIAL_CAPACITY;

    if ((a->tab = (char**)malloc(sizeof(char*) * a->capacity)))
        return a;
    else{
        free(a);
        perror("Error creating an array.");
        exit(EXIT_FAILURE);
    }
}

static void array_resize(array* a, int capacity){
    char **tab = (char**)realloc(a->tab, sizeof(char *) * capacity);
    if (tab) {
        a->tab = tab;
        a->capacity = capacity;
    } else {
        perror("Error resizing an array : ");
        exit(EXIT_FAILURE);
    }
}

short array_add(array* a, char* s){
    if (a != NULL){
        if (a->size == a->capacity)
            array_resize(a, 2 * a->size);
        a->tab[(a->size)++] = s;
        return 1;
    }
    return 0;
}

short array_removeIndex(array* a, int i){
    if (a == NULL || i < 0 || i >= a->size)
        return 0;
    --(a->size);
    for ( ; i < a->size ; i++)
        a->tab[i] = a->tab[i + 1];
    return 1;
}

short array_remove(array* a, char* s){
    if (a != NULL)
        for (int i = 0; i < a->size; i++)
            if (a->tab[i] == s)
                return array_removeIndex(a, i);
    return 0;
}

short array_set(array* a, int i, char* s){
    if (a == NULL || i < 0 || i >= a->size)
        return 0;
    a->tab[i] = s;
    return 1;
}

int array_size(array* a){
    return a == NULL ? 0 : a->size;
}

int array_index(array* a, char* s){
    if (a != NULL)
        for (int i = 0; i < a->size; i++)
            if (a->tab[i] == s)
                return i;
    return -1;
}

short array_contains(array* a, char* s){
    return array_index(a, s) != -1;
}
