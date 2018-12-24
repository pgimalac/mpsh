#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "command.h"
#include "lp/parser.h"
#include "builtin.h"
#include "array.h"
#include "completion.h"

void exec_simple_redir (struct simple_redir *red) {
    red++;
}

void exec_file_redir (struct file_redir *red) {
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

void exec_redirections (struct cmd_s *cmd) {
    for (list_t *r = cmd->redirs; r; r = r->next) {
        struct redir *red = (struct redir*)r->val;
        if (red->is_simple) exec_simple_redir (red->sredir);
        else exec_file_redir (red->fredir);
    }
}

unsigned char exec_simple (struct cmd_s *cmd) {
    if (is_builtin(cmd->argv[0]))
        return exec_builtin(cmd);

    char* path = find_cmd(cmd->argv[0]);

    if (!path){
        fprintf(stderr, "mpsh: can't find %s\n", cmd->argv[0]);
        return 1;
    }

    int pid, status;

    if ((pid = fork()) == -1) {
        perror("mpsh");
        return 1;
    }

    if (pid == 0) {
        // child process
        exec_redirections (cmd);
        if (execv(path, cmd->argv) == -1) {
            perror("mpsh");
            return 1;
        }
        return 0;
    }

    waitpid(pid, &status, 0);
    free(path);
    return status;
}

unsigned char add_variable (struct var_d *var) {
    if (!hashmap_add(vars, var->name, var->value, 1)) {
        fprintf(stderr, "an error occured while assigniating variable %s\n", var->name);
        return 1;
    }

    return 0;
}

// TODO: dont work
unsigned char exec_pipe (cmd_t *left, cmd_t *right) {
    int fds[2], pid;

    if (pipe(fds) != 0) {
        perror("mpsh");
        return 1;
    }
    if ((pid = fork()) == -1) {
        perror("mpsh");
        return 1;
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
    case PIPE:
        return exec_pipe(cmd->left, cmd->right);
    default:
        return 0;
    }
}

unsigned char exec_cmd (cmd_t *cmd) {
    if (!cmd) return 0;
    switch (cmd->type) {
    case SIMPLE:
        return exec_simple(cmd->cmd_sim);
    case BIN:
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
            struct redir *r = (redir*)e->val;
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

static char* strapp(char* str1, char* str2, char* str3){
    int l1 = strlen(str1);
    int l2 = strlen(str2);
    int l3 = strlen(str3);
    char* str = malloc(sizeof(char*) * (l1 + l2 + l3 + 1));
    if (str){
        strcpy(str, str1);
        strcpy(str + l1, str2);
        strcpy(str + l1 + l2, str3);
    }
    return str;
}

static char* search_dir(char* st, char* path, short rec){
    DIR* dir = opendir(path);
    if (!dir){
        perror("mpsh");
        return NULL;
    }

    char* ret = NULL;
    struct dirent *entry;
    while (!ret && (entry = readdir(dir)))
        if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name))
            continue;
        else if (entry->d_type == DT_REG) {
            if (!strcmp(st, entry->d_name))
                ret = strapp(path, "/", st);
        } else if (rec && entry->d_type == DT_DIR)
            ret = search_dir(st, strapp(path, "/", entry->d_name), rec);

    free(path);
    closedir(dir);
    return ret;
}

char* find_cmd(char* st){
    char* var_path = hashmap_get(vars, "CHEMIN");
    if (!var_path)
        return NULL;
    var_path = strdup(var_path);

    int l;
    short rec = 0;
    char* buff = NULL;

    for (char* path = strtok(var_path, ":"); path && !buff; path = strtok(NULL, ":")) {
        l = strlen(path);
        if (path[l - 1] == '/'){
            path[l - 1] = '\0';
            if (path[l - 2] == '/'){
                path[l - 2] = '\0';
                rec = 1;
            }
        }

        buff = search_dir(st, strapp(path, "/", ""), rec);
    }

    free(var_path);
    return buff;
}

short is_cmd(char* st){
    char* s = find_cmd(st);
    short ret = s != NULL;
    free(s);
    return ret;
}

