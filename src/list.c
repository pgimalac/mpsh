#include "list.h"

list_t* list_init(void *v){
    list_t* l = malloc(sizeof(list_t));
    if (l) l->val = v;
    return l;
}

list_t* list_add(list_t* l, void* s){
    list_t* e = list_init(s);
    e->next = l;
    return e;
}

list_t* list_remove(list_t* l, int i){
    if (l == NULL || i < 0)
        return l;
    list_t* t = l;
    if (i == 0){
        l = l->next;
        free(t);
    } else {
        for (; i != 1 && t != NULL; t = t->next)
            i--;
        list_t* tmp = t->next;
        t->next = tmp->next;
        free(tmp);
    }
    return l;
}

void list_set(list_t* l, int i, void* v){
    if (l != NULL && i >= 0){
        for (; i != 0 && l != NULL; l = l->next) ;
        if (i == 0 && l != NULL)
            l->val = v;
    }
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
