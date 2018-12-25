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
