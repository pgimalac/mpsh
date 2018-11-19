#include "hashset.h"

#define HASHSET_INITIAL_CAPACITY 128
#define HASHSET_RATIO_UPPER_LIMIT 0.8
#define HASHSET_RATIO_LOWER_LIMIT 0.1

hashset_t* hashset_init(){
    hashset_t *h = malloc(sizeof(hashset_t));
    if (h){
        h->size = 0;
        h->capacity = HASHSET_INITIAL_CAPACITY;
        if ((h->tab = malloc(sizeof(list_t*) * h->capacity)))
            for(int i = 0; i < h->capacity; i++)
                h->tab[i] = NULL;
        else {
            free(h);
            h = NULL;
        }
    }
    return h;
}

// djb2 function from http://www.cse.yorku.ca/~oz/hash.html
static unsigned int hash(char *s) {
    unsigned int hash = 5381;
    int c;
    while ((c = *s++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

static short resize(hashset_t* h, int capacity){
    list_t** t;
    if (!(t = malloc(sizeof(list_t) * capacity)))
        return 0;

    for (int i = 0; i < capacity; i++)
        t[i] = NULL;

    for (int i = 0; i < h->capacity; i++, list_destroy(h->tab[i]))
        for (list_t* l = h->tab[i]; l != NULL; l = l->next)
            t[i] = list_addFirst(t[i], l->val);

    free(h->tab);
    h->capacity = capacity;
    h->tab = t;

    return 1;
}

short hashset_contains(hashset_t* h, char* s){
    if (h != NULL)
        for (list_t* l = h->tab[hash(s) % h->capacity]; l != NULL; l = l->next)
            if (strcmp(l->val, s) == 0)
                return 1;
    return 0;
}

short hashset_add(hashset_t* h, char* s){
    if (h != NULL){
        if(!hashset_contains(h, s)){
            if (h->size + 1 > HASHSET_RATIO_UPPER_LIMIT * h->capacity)
                resize(h, h->capacity * 2);
            h->tab[hash(s) % h->capacity] = list_addFirst(h->tab[hash(s) % h->capacity], s);
            h->size ++;
            return 1;
        }
    }
    return 0;
}

static list_t* hashset_list_remove(list_t* l, char* s){
    if (l != NULL){
        if (strcmp(l->val, s) == 0)
            return list_remove(l, 0);

        list_t* tmp;
        for(tmp = l; tmp->next != NULL && strcmp(s, tmp->next->val); tmp = tmp->next) ;

        tmp->next = list_remove(tmp->next, 0);
    }
    return l;
}

short hashset_remove(hashset_t* h, char* s){
    if (h != NULL && hashset_contains(h, s)){
        int i = hash(s) % h->capacity;
        h->tab[i] = hashset_list_remove(h->tab[i], s);
        h->size--;
        if (h->size < HASHSET_RATIO_LOWER_LIMIT * h->capacity && h->size > HASHSET_INITIAL_CAPACITY)
            resize(h, h->capacity / 2);
        return 1;
    }
    return 0;
}

void hashset_destroy(hashset_t* h){
    for (int i = 0; i < h->capacity; i++)
        list_destroy(h->tab[i]);
    free(h->tab);
    free(h);
}
