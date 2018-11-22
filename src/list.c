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

    if (k == i) *l = (*l)->next;
    return 1;
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

void list_destroy(list_t* l){
    if (l != NULL){
        list_destroy(l->next);
        free(l);
    }
}
