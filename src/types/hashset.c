#include "hashset.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "utils.h"

#define HASHSET_INITIAL_CAPACITY 16
#define HASHSET_RATIO_UPPER_LIMIT 0.8
#define HASHSET_RATIO_LOWER_LIMIT 0.1

hashset_t* hashset_init(){
    hashset_t *h = malloc(sizeof(hashset_t));
    if (h){
        h->size = 0;
        h->capacity = HASHSET_INITIAL_CAPACITY;
        h->tab = calloc(h->capacity, sizeof(list_t*));
        if (!h->tab){
            free(h);
            h = NULL;
        }
    }
    return h;
}

static short resize(hashset_t* h, int capacity){
    list_t** t = calloc(capacity, sizeof(list_t));
    if (!t) return 0;

    for (int i = 0; i < h->capacity; list_destroy(h->tab[i], 0), i++)
        for (list_t* l = h->tab[i]; l != NULL; l = l->next)
            list_add(&t[hash((char *)l->val) % capacity], l->val);

    free(h->tab);
    h->capacity = capacity;
    h->tab = t;

    return 1;
}

short hashset_contains(hashset_t* h, char* s){
    if (h == NULL) return 0;

    for (list_t* l = h->tab[hash(s) % h->capacity]; l != NULL; l = l->next)
        if (strcmp(l->val, s) == 0)
            return 1;

    return 0;
}

short hashset_add(hashset_t* h, char* s){
    if (h == NULL) return 0;

    if(!hashset_contains(h, s)){
        if (h->size + 1 > HASHSET_RATIO_UPPER_LIMIT * h->capacity)
            resize(h, h->capacity * 2);
        list_add(&h->tab[hash(s) % h->capacity], s);
        h->size ++;
        return 1;
    }

    return 0;
}

static short hashset_list_remove(list_t** l, char* s){
    if (l != NULL){
        if (strcmp((*l)->val, s) == 0){
            list_remove(l, 0);
            return 1;
        }

        list_t* tmp;
        for(tmp = *l; tmp->next != NULL && strcmp(s, tmp->next->val); tmp = tmp->next) ;

        list_remove(&tmp->next, 0);
    }
    return 1;
}

short hashset_remove(hashset_t* h, char* s){
    if (h != NULL && hashset_contains(h, s)){
        int i = hash(s) % h->capacity;
        hashset_list_remove(&h->tab[i], s);
        h->size--;
        if (h->size < HASHSET_RATIO_LOWER_LIMIT * h->capacity &&
            h->size > HASHSET_INITIAL_CAPACITY)
            resize(h, h->capacity / 2);
        return 1;
    }
    return 0;
}

void hashset_destroy(hashset_t* h, short f){
    if (h == 0) return;

    for (int i = 0; i < h->capacity; i++)
        list_destroy(h->tab[i], f);
    free(h->tab);
    free(h);
}
