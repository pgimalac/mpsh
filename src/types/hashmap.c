#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "list.h"
#include "utils.h"

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

static short resize(hashmap_t *map, int capacity) {
    map_elem *e;
    list_t **tab = calloc(capacity, sizeof(list_t *));
    if (!tab)
        return 0;

    for (int i = 0; i < map->capacity; i++)
        for (list_t *l = map->tab[i]; l;) {
            e = (map_elem *)list_pop(&l);
            if (!list_add(&tab[hash(e->key) % map->capacity], e)) {
                free(tab);
                return 0;
            }
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

        map->tab = calloc(map->capacity, sizeof(list_t *));
        if (!map->tab) {
            free(map);
            map = NULL;
        }
    }

    return map;
}

static map_elem *get(hashmap_t *map, char *key) {
    if (map == NULL)
        return NULL;

    map_elem *e;
    for (list_t *l = map->tab[hash(key) % map->capacity]; l; l = l->next) {
        e = (map_elem *)l->val;
        if (strcmp(e->key, key) == 0)
            return e;
    }

    return 0;
}

short hashmap_add(hashmap_t *map, char *key, char *value, short f) {
    if (map == 0)
        return 0;

    map_elem *e = get(map, key);
    if (!e) {
        if (map->size + 1 > HASHMAP_RATIO_UPPER_LIMIT * map->capacity)
            if (!resize(map, map->capacity * 2))
                return 0;

        e = elem(key, value);
        return e && list_add(&map->tab[hash(key) % map->capacity], e);
    }

    if (f) {
        free(e->key);
        free(e->value);
    }
    e->key = key;
    e->value = value;

    return 1;
}

char *hashmap_get(hashmap_t *map, char *key) {
    struct map_elem *e = get(map, key);
    return e ? e->value : NULL;
}

static short map_list_remove(list_t **lst, char *key, short f) {
    if (!lst || !*lst)
        return 0;

    list_t *tmp;
    if (strcmp(key, ((map_elem *)(*lst)->val)->key) == 0) {
        tmp = (*lst);
        *lst = (*lst)->next;
    } else {
        while ((*lst)->next &&
               strcmp(key, ((map_elem *)(*lst)->next->val)->key))
            lst = &(*lst)->next;

        if (!*lst)
            return 0;

        tmp = (*lst)->next;
        (*lst)->next = tmp->next;
    }
    if (f) {
        free(((map_elem *)tmp->val)->key);
        free(((map_elem *)tmp->val)->value);
    }
    free(tmp->val);
    free(tmp);
    return 1;
}

short hashmap_remove(hashmap_t *map, char *key, short f) {
    if (!map)
        return 0;
    return map_list_remove(&map->tab[hash(key) % map->capacity], key, f);
}

short hashmap_contains(hashmap_t *map, char *key) { return get(map, key) != 0; }

void hashmap_destroy(hashmap_t *map, short f) {
    if (map == 0)
        return;

    for (int i = 0; i < map->capacity; i++)
        for (list_t *l = map->tab[i]; l; free(list_pop(&l)))
            if (f) {
                free(((map_elem *)l->val)->key);
                free(((map_elem *)l->val)->value);
            }
    free(map->tab);
    free(map);
}

void hashmap_iterate(hashmap_t *map, int fd, void (*f)(int, char *, char *)) {
    if (!map)
        return;

    map_elem *e;
    for (int i = 0; i < map->capacity; i++)
        for (list_t *l = map->tab[i]; l; l = l->next) {
            e = (map_elem *)l->val;
            f(fd, e->key, e->value);
        }
}

static void map_elem_print(int fd, char *k, char *v) {
    dprintf(fd, "%s: %s\n", k, v);
}

void hashmap_print(hashmap_t *map, int stdout) {
    hashmap_iterate(map, stdout, map_elem_print);
}
