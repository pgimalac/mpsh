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
#include "types/hashset.h"
#include "lp/parser.h"
#include "builtin.h"
#include "types/array.h"
#include "completion.h"
#include "env.h"
#include "types/list.h"
#include "utils.h"

#define REGISTER_TABLE_SIZE 3

extern hashmap_t *aliases;

struct state {
    int to_close[10], close_index;
    int fork;
    int fds[REGISTER_TABLE_SIZE];
};

struct state *create_state () {
    struct state *st = malloc(sizeof(struct state));
    if (st) {
        for (int i = 0; i < REGISTER_TABLE_SIZE; i++) st->fds[i] = -1;
        st->fds[0] = 0;
        st->fds[1] = 1;
        st->fds[2] = 2;
        st->fork = 1;
        st->close_index = 0;
    }

    return st;
}

list_t *bgps = 0;

static int g_pid = 0;
int seek_pid (int *a) {
    return g_pid == *a;
}

void sigchild_handler (int sig) {
    int pid, status;
    if ((pid = wait(&status)) == -1) return;
    g_pid = pid;
    int len = list_size(bgps);
    int index = list_filter(&bgps, (int(*)(void*))seek_pid);
    if (index == -1) return;
    printf("\n[%d] %d %d\n", len - index, pid, status);
}

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

unsigned char exec_simple (struct cmd_s *cmd, struct state *st) {
    if (is_builtin(cmd->argv[0])) {
        exec_redirections(cmd->redirs, st->fds);
        return exec_builtin(cmd, st->fds[0], st->fds[1], st->fds[2]);
    }

    char* path = find_cmd(cmd->argv[0]);

    if (!path){
        fprintf(stderr, "mpsh: can't find %s\n", cmd->argv[0]);
        return 1;
    }

    int pid = 0, status;

    exec_redirections (cmd->redirs, st->fds);
    if (st->fork && ((pid = fork()) == -1)) {
        perror("mpsh: Fork error");
        free(path);
        return 1;
    }

    if (pid == 0) {
        dup2(st->fds[0], 0);
        dup2(st->fds[1], 1);
        dup2(st->fds[2], 2);
        execv(path, cmd->argv);
        perror("mpsh");
        return 1;
    }

    while (st->close_index > 0)
        close(st->to_close[--st->close_index]);

    if (cmd->bg) {
        int *p = malloc(sizeof(int));
        *p = pid;
        list_add(&bgps, p);
        printf("[%d] %d\n", list_size(bgps), pid);
    } else waitpid(pid, &status, 0);
    free(path);
    return WEXITSTATUS(status);
}

unsigned char add_variable (struct var_d *var) {
    add_var(strdup(var->name), strdup(var->value), 0);
    return 0;
}

unsigned char exec_with_redirections (cmd_t *cmd, struct state *st);

unsigned char exec_pipe (cmd_t *left, cmd_t *right, struct state *st) {
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
        close(fdpipe[0]);
        st->fds[1] = fdpipe[1];
        st->to_close[st->close_index++] = fdpipe[1];
        st->fork = 0;
        return exec_with_redirections(left, st);
    }

    st->to_close[st->close_index++] = fdpipe[0];
    close(fdpipe[1]);
    st->fds[0] = fdpipe[0];
    return exec_with_redirections(right, st);
}

unsigned char exec_with_redirections (cmd_t *cmd, struct state *st) {
    switch(cmd->type) {
    case SIMPLE:
        return exec_simple(cmd->cmd_sim, st);
    case BIN:
        // assert it is a pipe
        return exec_pipe(cmd->cmd_bin->left, cmd->cmd_bin->right, st);
    default:
        return 0;
    }
}

unsigned char exec_bin (struct cmd_b *cmd) {
    if (!cmd) return 0;
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

unsigned char exec_script(int fd){
    if (fd < 0) return 1;

    struct stat st;
    if (fstat(fd, &st) == -1){
        perror("mpsh script");
        close(fd);
        return 1;
    }

    if ((st.st_mode & S_IFMT) != S_IFREG){
        fprintf(stderr, "mpsh script: not a regular file");
        close(fd);
        return 1;
    }

    if (lseek(fd, 0, SEEK_SET) == -1){
        perror("mpsh script: can't move to the beginning of the file.");
        close(fd);
        return 1;
    }

    FILE* file = fdopen(fd, "r");
    if (!file){
        perror("mpshrc script");
        close(fd);
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, file)) != -1) {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = 0;
        command_line_handler(line);
        free(line);
        line = NULL;
        len = 0;
    }

    free(line);
    fclose(file);
    close(fd);
    return 0;
}

unsigned char exec_cmd (cmd_t *cmd) {
    if (!cmd) return 0;
    struct state *st = create_state();
    unsigned char ret;

    switch (cmd->type) {
    case SIMPLE:
        ret = exec_simple(cmd->cmd_sim, st);
        break;
    case BIN:
        if (cmd->cmd_bin->type == PIPE)
            ret = exec_pipe(cmd->cmd_bin->left, cmd->cmd_bin->right, st);
        else
            ret = exec_bin(cmd->cmd_bin);
        break;
    case VAR:
        ret = add_variable(cmd->cmd_var);
        break;
    default:
        fprintf(stderr, "unknow command type\n");
        ret = 1;
    }
    free(st);
    return ret;
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

void command_line_handler (char *input) {
    if (!input) return;

    yy_scan_string(input);
    if (yyparse() != 0) return;

    unsigned char ret = exec_cmd(parse_ret);

    add_var(strdup("?"), uchar_to_string(ret), 0);
    free_cmd_t(parse_ret);
    parse_ret = NULL;
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
                ret = strappl(path, "/", st, NULL);
        } else if (rec && entry->d_type == DT_DIR)
            ret = search_dir(st, strappl(path, "/", entry->d_name, NULL), rec);

    free(path);
    closedir(dir);
    return ret;
}

char* find_cmd(char* st){
    if (!st)
        return NULL;

    if (strrchr(st, '/') && is_valid_file_path(st))
        return st;

    char* var_path = get_var("CHEMIN");
    if (!var_path)
        return NULL;

    int l;
    short rec = 0;
    char* buff = NULL;

    for (char* path = strtok(var_path, ":"); path && !buff; path = strtok(NULL, ":")) {
        l = strlen(path);
        if (l >= 1 && path[l - 1] == '/'){
            path[l - 1] = '\0';
            if (l >= 2 && path[l - 2] == '/'){
                path[l - 2] = '\0';
                rec = 1;
            }
        }

        buff = search_dir(st, strdup(path), rec);
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
