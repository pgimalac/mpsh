#include <stdlib.h>

#include "list.h"
#include "parsing.h"

var_d* create_var_d (char *name, char *value) {
    var_d *v = malloc(sizeof(var_d));
    if (v) {
        v->name = name;
        v->value = value;
    }

    return v;
}

simple_redir* create_simple_redir (redir_t type, int fd1, int fd2) {
    simple_redir *red = malloc(sizeof(simple_redir));
    if (red) {
        red->type = type;
        red->fd1  = fd1;
        red->fd2  = fd2;
    }

    return red;
}

file_redir* create_file_redir (redir_t type, int fd1, char *name) {
    file_redir *red = malloc(sizeof(file_redir));
    if (red) {
        red->type  = type;
        red->fd    = fd1;
        red->fname = name;
    }

    return red;
}

redir* redir_from_simple (simple_redir *r) {
    redir *red = malloc(sizeof(redir));
    if (r) {
        red->sredir  = r;
        red->is_simple = 1;
    }

    return red;
}

redir* redir_from_file (file_redir *r) {
    redir *red = malloc(sizeof(redir));
    if (r) {
        red->fredir  = r;
        red->is_simple = 0;
    }

    return red;
}

cmd_s* create_cmd_s (char **argv, list_t *redirs) {
    cmd_s *f = malloc(sizeof(cmd_s));
    if (f) {
        f->argv = argv;
        f->redirs = redirs;
    }

    return f;
}

cmd_b* create_cmd_b (bin_op op, cmd_t *left, cmd_t *right) {
    cmd_b *b = malloc(sizeof(cmd_b));
    if (b) {
        b->type = op;
        b->left = left;
        b->right = right;
    }

    return b;
}

cmd_t* create_cmd_with_simple (cmd_s *s) {
    cmd_t *cmd = malloc(sizeof(cmd_t));
    if (cmd) {
        cmd->type = SIMPLE;
        cmd->cmd_sim = s;
    }

    return cmd;
}

cmd_t* create_cmd_with_bin_op (cmd_b *b) {
    cmd_t *cmd = malloc(sizeof(cmd_t));
    if (cmd) {
        cmd->type = BIN;
        cmd->cmd_bin = b;
    }

    return cmd;
}

cmd_t* create_cmd_with_var_def (var_d *v) {
    cmd_t *cmd = malloc(sizeof(cmd_t));
    if (cmd) {
        cmd->type = VAR;
        cmd->cmd_var = v;
    }

    return cmd;
}

void free_var_d(var_d* v){
    free(v->name);
    free(v->value);
    free(v);
}

void free_simple_redir(simple_redir* s){
    free(s);
}

void free_file_redir(file_redir* s){
    free(s->fname);
    free(s);
}

void free_redir (redir* r){
    if (r->is_simple)
        free(r->sredir);
    else
        free(r->fredir);
    free(r);
}

void free_cmd_s(cmd_s* c){
    for (char** tmp = c->argv; *tmp; tmp++)
        free(*tmp);
    free(c->argv);
    for (list_t* l = c->redirs; l; l = l->next)
        free_redir((redir*)l->val);
    list_destroy(c->redirs);
    free(c);
}

void free_cmd_b(cmd_b* c){
    free_cmd_t(c->left);
    free_cmd_t(c->right);
    free(c);
}

void free_cmd_t(cmd_t* c){
    if (c->type == BIN)
        free_cmd_b(c->cmd_bin);
    else if (c->type == SIMPLE)
        free_cmd_s(c->cmd_sim);
    else if (c->type == VAR)
        free_var_d(c->cmd_var);
    free(c);
}
