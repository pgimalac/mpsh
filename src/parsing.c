#include <stdlib.h>

#include "list.h"
#include "parsing.h"

struct var_d *
create_var_d (char *name, char *value) {
    struct var_d *v = malloc(sizeof(struct var_d));
    if (v) {
        v->name = name;
        v->value = value;
    }

    return v;
}

struct simple_redir *
create_simple_redir (redir_t type, int fd1, int fd2) {
    struct simple_redir *red = malloc(sizeof(struct simple_redir));
    if (red) {
        red->type = type;
        red->fd1  = fd1;
        red->fd2  = fd2;
    }

    return red;
}

struct file_redir *
create_file_redir (redir_t type, int fd1, char *name) {
    struct file_redir *red = malloc(sizeof(struct file_redir));
    if (red) {
        red->type  = type;
        red->fd    = fd1;
        red->fname = name;
    }

    return red;
}

struct redir *
redir_from_simple (struct simple_redir *r) {
    struct redir *red = malloc(sizeof(struct redir));
    if (r) {
        red->sredir  = r;
        red->is_simple = 1;
    }

    return red;
}

struct redir *
redir_from_file (struct file_redir *r) {
    struct redir *red = malloc(sizeof(struct redir));
    if (r) {
        red->fredir  = r;
        red->is_simple = 0;
    }

    return red;
}

struct cmd_s *
create_cmd_s (char **argv, list_t *redirs) {
    struct cmd_s *f = malloc(sizeof(struct cmd_s));
    if (f) {
        f->argv = argv;
        f->redirs = redirs;
    }

    return f;
}

struct cmd_b *
create_cmd_b (bin_op op, cmd_t *left, cmd_t *right) {
    struct cmd_b *b = malloc(sizeof(struct cmd_b));
    if (b) {
        b->type = op;
        b->left = left;
        b->right = right;
    }

    return b;
}

cmd_t *
create_cmd_with_simple (struct cmd_s *s) {
    cmd_t *cmd = malloc(sizeof(cmd_t));
    if (cmd) {
        cmd->type = SIMPLE;
        cmd->cmd_sim = s;
    }

    return cmd;
}

cmd_t *
create_cmd_with_bin_op (struct cmd_b *b) {
    cmd_t *cmd = malloc(sizeof(cmd_t));
    if (cmd) {
        cmd->type = BIN;
        cmd->cmd_bin = b;
    }

    return cmd;
}

cmd_t *
create_cmd_with_var_def (struct var_d *v) {
    cmd_t *cmd = malloc(sizeof(cmd_t));
    if (cmd) {
        cmd->type = VAR;
        cmd->cmd_var = v;
    }

    return cmd;
}
