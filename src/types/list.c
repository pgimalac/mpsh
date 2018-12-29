#include <stdio.h>
#include <stdlib.h>

#include "list.h"

list_t* list_init(void *v, list_t* n){
    list_t* l = malloc(sizeof(list_t));
    if (l){
        l->val = v;
        l->next = n;
    }
    return l;
}

short list_add(list_t** l, void* s){
    list_t* tmp = list_init(s, *l);
    if (tmp){
        *l = tmp;
        return 1;
    }
    return 0;
}

short list_remove(list_t** l, int i){
    if (*l == NULL || i < 0)
        return 0;
    int k;

    for (k = 0; (*l)->next && k < i; k++)
        l = &(*l)->next;

    if (k == i){
        list_t* tmp = *l;
        *l = (*l)->next;
        free(tmp);
    }
    return 1;
}

short list_filter(list_t **l, int(*pred)(void*)) {
    if (*l == 0) return 0;
    int i = 0, k = 0;

    while (*l) {
        if (pred((*l)->val)) {
            i = k;
            *l = (*l)->next ? (*l)->next->next : 0;
        } else *l = (*l)->next;
        k++;
    }

    return i;
}

void* list_pop(list_t** l){
    void* e = NULL;
    if (l && *l){
        e = (*l)->val;
        list_t* tmp = *l;
        *l = (*l)->next;
        free(tmp);
    }
    return e;
}

short list_set(list_t* l, int i, void* v){
    if (l != NULL && i >= 0){
        for (; i != 0 && l != NULL; l = l->next) ;
        if (i == 0 && l != NULL){
            l->val = v;
            return 1;
        }
    }
    return 0;
}

int list_size(list_t* l){
    int i = 0;
    for (; l != NULL; l = l->next)
        i++;
    return i;
}

void list_destroy(list_t* l, short f){
    if (l != NULL){
        list_destroy(l->next, f);
        if (f) free(l->val);
        free(l);
    }
}

void ** list_to_tab(list_t *list, size_t sz) {
    int length = list_size(list) + 1;
    void **tab = malloc(sz * length);
    void **tabi = tab;

    while (list) {
        list_t *tmp = list;
        *tabi = list->val;
        tabi = (void**)((char*)tabi + sz);
        list = list->next;
        free(tmp);
    }

    *tabi = 0;

    return tab;
}