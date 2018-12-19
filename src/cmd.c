#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "hashmap.h"
#include "parsing.h"
#include "cmd.h"

extern hashmap_t *vars;

void
print_cmd (cmd_t *cmd) {
    if (!cmd) return;
    switch (cmd->type) {
    case SIMPLE:
        printf("simple: ");
        for (int i = 0; i < cmd->cmd_s->argc; i++)
            printf("%s, ", cmd->cmd_s->argv[i]);
        printf("\n");
        printf("redirections: \n");
        for (list_t *e = cmd->cmd_s->redirs; e; e = e->next) {
            struct redir *r = e->val;
            if (r->is_simple)
                printf("-> %d %d %d\n", r->sredir->type, r->sredir->fd1, r->sredir->fd2);
            else
                printf("-> %d %d %s\n", r->fredir->type, r->fredir->fd, r->fredir->fname);
        }
        break;
    case BIN:
        printf("bin %d\n", cmd->cmd_b->type);
        print_cmd(cmd->cmd_b->left);
        print_cmd(cmd->cmd_b->right);
        break;
    case VAR:
        printf("var\n");
        printf("%s '%s'\n", cmd->cmd_v->name, cmd->cmd_v->value);
        break;
    default:
        printf("unknow\n");
    }
}

void
make_simple_redir (struct simple_redir *red) {
    red++;
}

void
make_file_redir (struct file_redir *red) {
    // TODO: change open permissions
    int fd = open(red->fname, O_RDWR|O_TRUNC|O_CREAT, 0644);
    if (fd == -1) {
        perror("mpsh: can't create file");
        return;
    }

    int fdold, fdnew;

    printf("%d\n", red->type);
    switch (red->type) {
    case REDIR_WRITE:
        fdold = 1;
        fdnew = fd;
        break;
    case REDIR_APP:
        lseek(fd, 0, SEEK_END);
        fdold = 1;
        fdnew = fd;
        break;
    case REDIR_ERRWRITE:
        fdold = red->fd;
        fdnew = fd;
        break;
    case REDIR_READ:
        fdold = 0;
        fdnew = fd;
        break;
    default:
        fdold = 1;
        fdnew = fd;
    }

    if (dup2(fdnew, fdold) == -1) {
        perror("mpsh: cant redirect streams");
    }
}

void
make_redirections (struct cmd_s *cmd) {
    for (list_t *r = cmd->redirs; r; r = r->next) {
        struct redir *red = (struct redir*)r->val;
        if (red->is_simple) make_simple_redir (red->sredir);
        else make_file_redir (red->fredir);
    }
}

int
make_simple (struct cmd_s *cmd) {
    int pid, status;

    if ((pid = fork()) == -1) {
        perror("mpsh");
        return -1;
    }

    if (pid == 0) {
        // child process
        make_redirections (cmd);
        if (execvp(cmd->argv[0], cmd->argv) == -1) {
            perror("mpsh:");
            return -1;
        }
        return 0;
    }

    waitpid(pid, &status, 0);
    return status;
}

int
add_variable (struct var_d *var) {
    if (hashmap_add(vars, var->name, var->value) == -1) {
        fprintf(stderr, "an error occured while assigniating variable %s\n", var->name);
        return 1;
    }

    return 0;
}

// TODO: dont work
int
make_pipe (cmd_t *left, cmd_t *right) {
    int fds[2], pid;

    if (pipe(fds) != 0) {
        perror("mpsh:");
        return -1;
    }

    if ((pid = fork()) == -1) {
        perror("mpsh:");
        return -1;
    }

    if (pid == 0) {
        close(fds[0]);
        dup2(fds[1], 1);
        return exec_cmd(left);
    }

    close(fds[1]);
    dup2(fds[0], 0);
    return exec_cmd(right);
}

int
make_bin (struct cmd_b *cmd) {
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
    case PIPE:
        return make_pipe(cmd->left, cmd->right);
    default:
        return 0;
    }
}

int
exec_cmd (cmd_t *cmd) {
    if (!cmd) return 0;
    switch (cmd->type) {
    case SIMPLE:
        return make_simple(cmd->cmd_s);
    case BIN:
        return make_bin(cmd->cmd_b);
    case VAR:
        return add_variable(cmd->cmd_v);
    default:
        fprintf(stderr, "unknow command type\n");
        return 1;
    }
}
