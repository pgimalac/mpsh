#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "parsing.h"
#include "parser.h"

extern int yy_scan_string(const char *);
extern cmd_t *parse_ret;

char *dupstr(char *s){
    char *r = malloc(strlen(s)+1);
    strcpy (r, s);
    return r;
}

#define MAX_COMPLETION 10000

char *completions[MAX_COMPLETION];
int nb_completions;

void init_completion () {
    DIR *dir;
    struct dirent *entry;
    char *var_path = getenv("PATH");
    char *path, *buf, *pathname;
    char *saveptr1, *saveptr2;
    struct stat stat_buf, stat_buf2;

    nb_completions = 0;
    for (int i = 0; i < MAX_COMPLETION; i++)
       completions[i] = 0;

    for (path = strtok_r(var_path, ":", &saveptr1);; var_path = 0,
         path = strtok_r(0, ":", &saveptr1)) {
        if (!path) break;

        if (stat(path, &stat_buf) == 0) {
            if (S_ISDIR(stat_buf.st_mode)) {

                if ((dir = opendir(path))) {
                    while (nb_completions < MAX_COMPLETION &&
                       (entry = readdir(dir)))

                        if (entry->d_type == DT_REG) { // TODO: check if exec
                            pathname = malloc(sizeof(char) * 512);
                            strncpy(pathname, path, 256);
                            strcat(pathname, "/");
                            strncat(pathname, entry->d_name, 256);

                            if(stat(pathname, &stat_buf2) == 0 &&
                               stat_buf2.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                                buf = malloc(sizeof(char) * 256);
                                strncpy(buf, entry->d_name, 255);
                                completions[nb_completions++] = buf;
                            }

                            free(pathname);
                        }
                } else fprintf(stderr, "Can't open %s", path);

                closedir(dir);
            } else if (nb_completions < MAX_COMPLETION &&
                   S_ISREG(stat_buf.st_mode) &&
                   stat_buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                char *name, *tmp;
                for (tmp = strtok_r(path, "/", &saveptr2); tmp; path = 0,
                     tmp = strtok_r(0, "/", &saveptr2))
                    name = tmp;

                completions[nb_completions++] = name;
            }
        } else fprintf(stderr, "Can't open %s", path);
    }
}

char *command_generator (const char *com, int num){
    static int indice, len;
    char *completion;

    if (num == 0){
        indice = 0;
        len = strlen(com);
    }

    while (indice < nb_completions) {
        completion = completions[indice++];

        if (strncmp (completion, com, len) == 0)
            return dupstr(completion);
    }

    return NULL;
}

char ** fileman_completion (const char *com, int start, int end) {
    char **matches;
    matches = (char **)NULL;
    end = end;

    if (start == 0)
       matches = rl_completion_matches (com, command_generator);

    return matches;
}

int init_readline() {
    rl_readline_name = "mpsh";

    rl_attempted_completion_function = fileman_completion;
    return 0;
}

void print_cmd (cmd_t *cmd) {
    if (!cmd) return;
    switch (cmd->type) {
        case FRA:
            printf("frag: ");
            for (int i = 0; i < cmd->cmd_f->argc; i++)
                printf("%s ", cmd->cmd_f->argv[i]);
            printf("\n");
            break;
        case BIN:
            switch (cmd->cmd_b->type) {
                case REDIR:
                    printf("redir %d from %d to %d\n",
                       cmd->cmd_b->redir->type,
                       cmd->cmd_b->redir->fd1,
                       cmd->cmd_b->redir->fd2);
                break;
                default:
                    printf("bin %d\n", cmd->cmd_b->type);
            }
            print_cmd(cmd->cmd_b->left);
            print_cmd(cmd->cmd_b->right);
            break;
        case VAR:
            printf("var\n");
            break;
        default:
            printf("unknow\n");
    }
}

void command_line_handler (char* input) {
    if (!input) return;

    char *dest = malloc(strlen(input) + 2);
    strcpy(dest, input);
    strcat(dest, " ");

    yy_scan_string(dest);
    if (yyparse() != 0) return;

    print_cmd(parse_ret);
}

int main (void) {
    char *s;

    init_completion();
    init_readline();

    while(1) {
        s = readline ("mpsh> ");
        command_line_handler(s);
        add_history(s);
        free(s);
    }

    return 0;
}
