#include <stdlib.h>

#include "types.h"

str_vec_t *create_str_vec() {
    str_vec_t *ret = malloc(sizeof(str_vec_t));
    if (ret) {
	ret->capacity = 4;
	ret->size = 0;
	ret->tab = malloc(sizeof(char *) * ret->capacity);
    }

    return ret;
}

void str_vec_add (str_vec_t *vec, char *str) {
    vec->tab[vec->size++] = str;
    
    if (vec->size == vec->capacity) {
	vec->capacity *= 2;
	vec->tab = realloc(vec->tab, vec->capacity);
    }
}

void str_vec_remove (str_vec_t *vec, int i) {
    char *tmp;
    
    while (i++ < vec->size - 1) {
	tmp = vec->tab[i + 1];
	vec->tab[i] = vec->tab[i + 1];
	vec->tab[i + 1] = tmp;
    }

    vec->size--;
    if (vec->size * 4 < vec->capacity) {
	vec->capacity /= 4;
	vec->tab = realloc(vec->tab, vec->capacity);
    }
}

str_list_t *create_str_list() {
    str_list_t *ret = malloc(sizeof(str_list_t));
    if (ret) {
	ret->length = 0;
	ret->first = 0;
	ret->last = 0;
    }

    return ret;
}

void add_first (str_list_t *lst, char **value) {
    struct str_list_node *node =
	malloc(sizeof(struct str_list_node));
    if (node) {
	node->value = value;
	node->next = lst->first;
	lst->first = node;
	if (node->next) lst->first->prev = node;
	else lst->last = node;
	lst->length++;
    }
}

void add_last (str_list_t *lst, char **value) {
    struct str_list_node *node =
	malloc(sizeof(struct str_list_node));
    if (node) {
	node->value = value;
	node->prev = lst->last;
	if (node->prev) lst->last->next = node;
	else lst->first = node;
	lst->length++;
    }
}

void free_str_list (str_list_t *lst) {
    struct str_list_node *node, *tmp;

    node = lst->first;
    while (node) {
	tmp = node;
	node = node->next;
	free(tmp);
    }

    free(lst);
}
