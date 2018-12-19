#include "command.h"

char exec_bin (struct cmd_b* cmd){
    if (!cmd)
        return 1;

    return 0;
}

char exec_sim (struct cmd_s* cmd){
    if (!cmd || !cmd->argv[0])
        return 1;

    if (is_built_in(cmd->argv[0]))
        return exec_builtin(cmd);

    return 1;
}

char exec_var (struct var_d* cmd){
    if (!cmd)
        return 1;

    return 0;
}

char exec_cmd (cmd_t *cmd){
    if (!cmd) return 1;

    switch (cmd->type) {
        case BIN:
            return exec_bin(cmd->cmd_bin);
        case SIMPLE:
            return exec_sim(cmd->cmd_sim);
        case VAR:
            return exec_var(cmd->cmd_var);

        default:
            return 1;
    }
}

void print_cmd (cmd_t *cmd) {
    if (!cmd) return;
    switch (cmd->type) {
    case SIMPLE:
        printf("simple: ");
        for (char* c = cmd->cmd_sim->argv[0]; c; c++)
            printf("%s, ", c);
        printf("\n");
        printf("redirections: \n");
        for (list_t *e = cmd->cmd_sim->redirs; e; e = e->next) {
            struct redir *r = e->val;
            if (r->is_simple)
                printf("-> %d %d %d\n", r->sredir->type, r->sredir->fd1, r->sredir->fd2);
            else
                printf("-> %d %d %s\n", r->fredir->type, r->fredir->fd, r->fredir->fname);
        }
        break;
    case BIN:
        printf("bin %d\n", cmd->cmd_bin->type);
        print_cmd(cmd->cmd_bin->left);
        print_cmd(cmd->cmd_bin->right);
        break;
    case VAR:
        printf("var\n");
        printf("%s '%s'\n", cmd->cmd_var->name, cmd->cmd_var->value);
        break;
    default:
        printf("unknow\n");
    }
}

void command_line_handler (char* input) {
    if (!input) return;

    yy_scan_string(input);
    if (yyparse() != 0) return;

    print_cmd(parse_ret);
    exec_cmd(parse_ret);
}
