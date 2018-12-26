#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "list.h"
#include "hashmap.h"

#define HASHMAP_INITIAL_CAPACITY 128
#define HASHMAP_RATIO_UPPER_LIMIT 0.8
#define HASHMAP_RATIO_LOWER_LIMIT 0.1

typedef struct map_elem {
    char *key;
    char *value;
} map_elem;

static map_elem *elem(char *key, char *value) {
    map_elem *e = malloc(sizeof(map_elem));
    if (e) {
        e->key = key;
        e->value = value;
    }

    return e;
}

// djb2 function from http://www.cse.yorku.ca/~oz/hash.html
// TODO: Factorize code with hashset
static unsigned int hash(char *s) {
    unsigned int hash = 5381;
    int c;
    while ((c = *s++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

static short resize (hashmap_t *map, int capacity) {
    map_elem *e;
    list_t **tab = malloc(sizeof(list_t *) * capacity);
    if (!tab) return 0;

    for (int i = 0; i < capacity; i++)
        tab[i] = NULL;

    for (int i = 0; i < map->capacity; i++)
        for (list_t *l = map->tab[i]; l; ) {
            e = (map_elem*)list_pop(&l);
            if (!list_add(&tab[hash(e->key) % map->capacity], e))
            return 0;
        }

    free(map->tab);
    map->capacity = capacity;
    map->tab = tab;
    return 1;
}

hashmap_t *hashmap_init() {
    hashmap_t *map = malloc(sizeof(hashmap_t));
    if (map) {
        map->size = 0;
        map->capacity = HASHMAP_INITIAL_CAPACITY;

        map->tab = malloc(sizeof(list_t *) * map->capacity);
        if (map->tab) {
            for (int i = 0; i < map->capacity; i++)
            map->tab[i] = 0;
        } else {
            free (map);
            map = 0;
        }
    }

    return map;
}

static map_elem *get (hashmap_t *map, char *key) {
    if (map == 0) return 0;

    map_elem *e;
    for (list_t *l = map->tab[hash(key) % map->capacity]; l; l = l->next) {
        e = (map_elem*)l->val;
        if (strcmp(e->key, key) == 0) return e;
    }

    return 0;
}

short hashmap_add(hashmap_t *map, char *key, char *value, short f) {
    if (map == 0) return 0;

    map_elem *e = get(map, key);
    if (!e) {
        if (map->size + 1 > HASHMAP_RATIO_UPPER_LIMIT * map->capacity)
            if (!resize(map, map->capacity * 2)) return 0;

        e = elem(key, value);
        return e && list_add(&map->tab[hash(key) % map->capacity], e);
    }

    if (f){
        free(e->key);
        free(e->value);
    }
    e->key = key;
    e->value = value;

    return 1;
}

char *hashmap_get(hashmap_t *map, char *key) {
    struct map_elem *e = get(map, key);
    return e ? e->value : 0;
}

static short map_list_remove (list_t **lst, char *key, short f) {
    if (!lst || !*lst)
        return 0;

    list_t* tmp;
    if (strcmp(key, ((map_elem*)(*lst)->val)->key) == 0){
        tmp = (*lst);
        *lst = (*lst)->next;
    } else {
        while ((*lst)->next && strcmp(key, ((map_elem*)(*lst)->next->val)->key))
            lst = &(*lst)->next;

        if (!*lst)
            return 0;

        tmp = (*lst)->next;
        (*lst)->next = tmp->next;
    }
    if (f){
        free(((map_elem*)tmp->val)->key);
        free(((map_elem*)tmp->val)->value);
    }
    free(tmp->val);
    free(tmp);
    return 1;
}

short hashmap_remove(hashmap_t *map, char *key, short f) {
    if (!map) return 0;
    return map_list_remove(&map->tab[hash(key) % map->capacity], key, f);
}

short hashmap_contains (hashmap_t *map, char *key) {
    return get(map, key) != 0;
}

void hashmap_destroy(hashmap_t *map, short f) {
    if (map == 0) return;

    for (int i = 0; i < map->capacity; i++)
        for (list_t* l = map->tab[i]; l; free(list_pop(&l)))
            if (f){
                free(((map_elem*)l->val)->key);
                free(((map_elem*)l->val)->value);
            }
    free(map->tab);
    free(map);
}

void hashmap_iterate (hashmap_t *map, void(*f)(char*, char*)){
    if (!map)
        return;

    map_elem *e;
    for (int i = 0; i < map->capacity; i++)
        for (list_t *l = map->tab[i]; l; l = l->next) {
            e = (map_elem*)l->val;
            f(e->key, e->value);
        }
}

static void map_elem_print (char* k, char* v){
    printf("%s: %s\n", k, v);
}

void hashmap_print (hashmap_t *map) {
    hashmap_iterate(map, map_elem_print);
}
