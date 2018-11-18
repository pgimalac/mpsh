#include "arrayList.h"

void arrayList_init(arrayList* a){
    a->size = 0;
    a->capacity = ARRAYLIST_INITIAL_CAPACITY;
    a->tab = (char**)malloc(sizeof(char*) * a->capacity);
}

static void arrayList_resize(arrayList* a, int capacity){
    char **tab = (char**)realloc(a->tab, sizeof(char *) * capacity);
    if (tab) {
        a->tab = tab;
        a->capacity = capacity;
    } else {
        perror("Error resizing an arrayList : ");
        exit(EXIT_FAILURE);
    }
}

short arrayList_add(arrayList* a, char* s){
    if (a != NULL){
        if (a->size == a->capacity)
            arrayList_resize(a, 2 * a->size);
        a->tab[(a->size)++] = s;
        return 1;
    }
    return 0;
}

short arrayList_removeIndex(arrayList* a, int i){
    if (a == NULL || i < 0 || i >= a->size)
        return 0;
    --(a->size);
    for ( ; i < a->size ; i++)
        a->tab[i] = a->tab[i + 1];
    return 1;
}

short arrayList_remove(arrayList* a, char* s){
    if (a != NULL)
        for (int i = 0; i < a->size; i++)
            if (a->tab[i] == s)
                return arrayList_removeIndex(a, i);
    return 0;
}

short arrayList_set(arrayList* a, int i, char* s){
    if (a == NULL || i < 0 || i >= a->size)
        return 0;
    a->tab[i] = s;
    return 1;
}

int arrayList_size(arrayList* a){
    return a == NULL ? 0 : a->size;
}

int arrayList_index(arrayList* a, char* s){
    if (a != NULL)
        for (int i = 0; i < a->size; i++)
            if (a->tab[i] == s)
                return i;
    return -1;
}

short arrayList_contains(arrayList* a, char* s){
    return arrayList_index(a, s) != -1;
}
