#include <stdlib.h>

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

struct cmd_f *
create_cmd_f (int argc, char **argv) {
    struct cmd_f *f = malloc(sizeof(struct cmd_f));
    if (f) {
	f->argc = argc;
	f->argv = argv;
    }

    return f;
}

struct cmd_b *
create_cmd_b (enum bin_op op, cmd_t *left, cmd_t *right) {
    struct cmd_b *b = malloc(sizeof(struct cmd_b));
    if (b) {
	b->op = op;
	b->left = left;
	b->right = right;
    }

    return b;
}

cmd_t *
create_cmd_with_frag (struct cmd_f *f) {
    cmd_t *cmd = malloc(sizeof(cmd_t));
    if (cmd) {
	cmd->type = FRA;
	cmd->cmd_f = f;
    }

    return cmd;
}

cmd_t *
create_cmd_with_bin_op (struct cmd_b *b) {
    cmd_t *cmd = malloc(sizeof(cmd_t));
    if (cmd) {
	cmd->type = BIN;
	cmd->cmd_b = b;
    }

    return cmd;
}

cmd_t *
create_cmd_with_var_def (struct var_d *v) {
    cmd_t *cmd = malloc(sizeof(cmd_t));
    if (cmd) {
	cmd->type = VAR;
	cmd->cmd_v = v;
    }
    
    return cmd;
}

