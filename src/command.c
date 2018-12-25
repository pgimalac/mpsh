#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "lp/parser.h"
#include "builtin.h"
#include "array.h"
#include "completion.h"

#define REGISTER_TABLE_SIZE 3

void exec_simple_redir (struct simple_redir *red, int fds[REGISTER_TABLE_SIZE]) {
    switch (red->type) {
    case REDIR_ERRWRITE:
        fds[red->fd1] = red->fd2;
        break;
    default:
        break;
    }
}

void exec_file_redir (struct file_redir *red, int fds[REGISTER_TABLE_SIZE]) {
    // TODO: change open permissions
    int fd = open(red->fname, O_RDWR|O_CREAT, 0644);
    if (fd == -1) {
        perror("mpsh: can't create file");
        return;
    }

    switch (red->type) {
    case REDIR_WRITE:
        fds[red->fd] = fd;
        break;
    case REDIR_APP:
        lseek(fd, 0, SEEK_END);
        fds[1] = fd;
        break;
    case REDIR_READ:
        fds[0] = fd;
        break;
    default:
        break;
    }
}

void exec_redirections (list_t *r, int fds[REGISTER_TABLE_SIZE]) {
    while (r) {
        struct redir *red = (struct redir*)r->val;
        if (red->is_simple) exec_simple_redir (red->sredir, fds);
        else exec_file_redir (red->fredir, fds);
        r = r->next;
    }
}

unsigned char exec_simple (struct cmd_s *cmd, int fds[REGISTER_TABLE_SIZE]) {
    if (is_builtin(cmd->argv[0])) {
        exec_redirections(cmd->redirs, fds);
        return exec_builtin(cmd, fds[0], fds[1], fds[2]);
    }

    int pid, status;

    exec_redirections (cmd->redirs, fds);
    if ((pid = fork()) == -1) {
        perror("mpsh: Fork error");
        return 1;
    }

    if (pid == 0) {
        dup2(fds[0], 0);
        dup2(fds[1], 1);
        dup2(fds[2], 2);
        if (execvp(cmd->argv[0], cmd->argv) == -1) {
            perror("mpsh");
            return 1;
        }
        return 0;
    }

    waitpid(pid, &status, 0);
    return status;
}

unsigned char add_variable (struct var_d *var) {
    if (hashmap_add(vars, var->name, var->value, 1) == -1) {
        fprintf(stderr, "an error occured while assigniating variable %s\n", var->name);
        return 1;
    }

    return 0;
}

unsigned char exec_with_redirections (cmd_t *cmd, int fds[REGISTER_TABLE_SIZE]);

// TODO: builtins in pipes dont work
unsigned char exec_pipe (cmd_t *left, cmd_t *right, int fds[REGISTER_TABLE_SIZE]) {
    int fdpipe[2], pid;

    if (pipe(fdpipe) != 0) {
        perror("mpsh: Broken pipe");
        return 1;
    }
    if ((pid = fork()) == -1) {
        perror("mpsh: Fork error");
        return 1;
    }

    if (pid == 0) {
        close(fdpipe[1]);
        fds[0] = fdpipe[0];
        return exec_with_redirections(right, fds);
    }

    close(fdpipe[0]);
    fds[1] = fdpipe[1];
    return exec_with_redirections(left, fds);
}

unsigned char exec_with_redirections (cmd_t *cmd, int fds[REGISTER_TABLE_SIZE]) {
    switch(cmd->type) {
    case SIMPLE:
        return exec_simple(cmd->cmd_sim, fds);
    case BIN:
        // assert it is a pipe
        return exec_pipe(cmd->cmd_bin->left, cmd->cmd_bin->right, fds);
    default:
        return 0;
    }
}

unsigned char exec_bin (struct cmd_b *cmd) {
    int left_status;

    switch (cmd->type) {
    case AND:
        left_status = exec_cmd(cmd->left);
        if (left_status != 0) return left_status;
        return exec_cmd(cmd->right);
    case OR:
        left_status = exec_cmd(cmd->left);
        if (left_status != 0) return exec_cmd(cmd->right);
        return left_status;
    case SEMI:
        exec_cmd(cmd->left);
        return exec_cmd(cmd->right);
    default:
        return 0;
    }
}

unsigned char exec_cmd (cmd_t *cmd) {
    int fds[REGISTER_TABLE_SIZE] = {-1};
    fds[0] = 0;
    fds[1] = 1;
    fds[2] = 2;

    if (!cmd) return 0;
    switch (cmd->type) {
    case SIMPLE:
        return exec_simple(cmd->cmd_sim, fds);
    case BIN:
        if (cmd->cmd_bin->type == PIPE)
            return exec_pipe(cmd->cmd_bin->left, cmd->cmd_bin->right, fds);
        return exec_bin(cmd->cmd_bin);
    case VAR:
        return add_variable(cmd->cmd_var);
    default:
        fprintf(stderr, "unknow command type\n");
        return 1;
    }
}

void print_cmd (cmd_t *cmd) {
    if (!cmd) return;
    switch (cmd->type) {
    case SIMPLE:
        printf("simple: ");
        for (int i = 0; cmd->cmd_sim->argv[i]; i++)
            printf("%s, ", cmd->cmd_sim->argv[i]);
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

    // print_cmd(parse_ret);
    exec_cmd(parse_ret);
}

short is_cmd(char* st){ // to improve !
    if (!getenv("PATH"))
        return 0;

    array_t* arr = get_all_files(getenv("PATH"));
    short s = array_contains(arr, st);
    array_destroy(arr, 1);
    return s;
}

char* find_cmd(char* st){ //TODO
    return strdup("TODO");
}
