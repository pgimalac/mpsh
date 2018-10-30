#include <stdio.h>
#include <stdlib.h>

#include "regexp.h"

/**
 * Construction of a non-deterministic finite automata
 * based on Thompson algorithm
 */
typedef struct nfa_state {
    int c;
    struct nfa_state *out1, *out2;
    int last_list;
} nfa_state;

#define SPLIT 256
#define MATCH 257

nfa_state* create_nfa_state (int c, nfa_state* out1, nfa_state* out2) {
    nfa_state* ret = malloc(sizeof(nfa_state));
    if (ret) {
	ret->c = c;
	ret->out1 = out1;
	ret->out2 = out2;
    }

    return ret;
}

typedef struct ptr_list {
    nfa_state** state;
    struct ptr_list* next;
} ptr_list;

ptr_list* list1(nfa_state** st) {
    ptr_list* lst = malloc(sizeof(ptr_list));
    if (lst) lst->state = st;

    return lst;
}

ptr_list* ptr_list_append (ptr_list* l1, ptr_list* l2) {
    if (!l1) return l2;
    
    while (l1->next) l1 = l1->next;
    l1->next = l2;
    return l1;
}

void patch (ptr_list* lst, nfa_state* s) {
    for (;lst; lst = lst->next)
	*lst->state = s;
}

typedef struct nfa_frag {
    nfa_state* start;
    ptr_list* out;
} nfa_frag;

nfa_frag* create_nfa_frag (nfa_state* start, ptr_list* out) {
    nfa_frag* ret = malloc(sizeof(nfa_frag));
    if (ret) {
	ret->start = start;
	ret->out = out;
    }

    return ret;
}

nfa_state* regexp_to_nfa (const char* regexp) {
    char c;
    nfa_frag *stack[1000], **stackp, *e1, *e2, *e;
    nfa_state* s;

#define push(s) *stackp++ = s
#define pop()   *--stackp

    stackp = stack;
    while ((c = *regexp++) != '\0')
	switch (c) {
	case '+':
	    e = pop();
	    s = create_nfa_state(SPLIT, e->start, NULL);
	    patch(e->out, s);
	    push(create_nfa_frag(e->start, list1(&s->out1)));
	    break;
	case '*':
	    e = pop();
	    s = create_nfa_state(SPLIT, e->start, NULL);
	    patch(e->out, s);
	    push(create_nfa_frag(s, list1(&s->out2)));
	    break;
	case '?':
	    e = pop();
	    s = create_nfa_state(SPLIT, e1->start, 0);
	    push(create_nfa_frag(s, ptr_list_append(e->out, list1(&s->out2))));
	    break;
	case '|':
	    e2 = pop();
	    e1 = pop();
	    s = create_nfa_state(SPLIT, e1->start, e2->start);
	    push(create_nfa_frag(s, ptr_list_append(e1->out, e2->out)));
	    break;
	case '.':
	    e2 = pop();
	    e1 = pop();
	    patch(e1->out, e2->start);
	    push(create_nfa_frag(e1->start, e2->out));
	    break;
	default:
	    s = create_nfa_state(c, 0, 0);
	    push(create_nfa_frag(s, list1(&s->out1)));
	    break;
	}

    e = pop();
    patch(e->out, create_nfa_state(MATCH, 0, 0));
    return e->start;
}

/**
 * Test if the given string matches the deterministic 
 * finite automata given by its initial state
 */
int match (const char* expr, state* st) {
    char c;
    while ((c = *expr++) != '\0')
	if (st->nexts[(int)c]) st = st->nexts[(int)c];
	else return 0;

    return st->is_terminal;
}
