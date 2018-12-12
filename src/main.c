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

#include "array.h"
#include "hashmap.h"
#include "parsing.h"
#include "parser.h"

extern int yy_scan_string(const char *);
extern cmd_t *parse_ret;

hashmap_t *vars;

char *dupstr(char *s){
    char *r = malloc(strlen(s) + 1);
    strcpy (r, s);
    return r;
}

char **completions;
int nb_completions;

void init_completion () {
    DIR *dir;
    struct dirent *entry;
    char *var_path = getenv("PATH");
    char *path, *buf, *pathname;
    char *saveptr1, *saveptr2;
    struct stat stat_buf, stat_buf2;

    array_t *array = array_init();

    for (path = strtok_r(var_path, ":", &saveptr1); path; var_path = 0,
             path = strtok_r(0, ":", &saveptr1)) {
        if (stat(path, &stat_buf) == 0) {
            if (S_ISDIR(stat_buf.st_mode)) {
                if ((dir = opendir(path))) {
                    while ((entry = readdir(dir)))
                        if (entry->d_type == DT_REG) {
                            pathname = malloc(sizeof(char) * 512);
                            strncpy(pathname, path, 256);
                            strcat(pathname, "/");
                            strncat(pathname, entry->d_name, 256);

                            if(stat(pathname, &stat_buf2) == 0 &&
                               stat_buf2.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {

                                buf = malloc(sizeof(char) * 256);
                                strncpy(buf, entry->d_name, 255);
                                array_add(array, buf);
                            }

                            free(pathname);
                        }
                } else fprintf(stderr, "Can't open %s", path);

                closedir(dir);
            } else if (S_ISREG(stat_buf.st_mode) &&
                       stat_buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                char *name, *tmp;
                for (tmp = strtok_r(path, "/", &saveptr2); tmp; path = 0,
                         tmp = strtok_r(0, "/", &saveptr2))
                    name = tmp;
                array_add(array, name);
            }
        } else fprintf(stderr, "Can't open %s", path);
    }

    nb_completions = array->size;
    completions = array_to_tab(array);
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

void command_line_handler (char* input) {
    if (!input) return;

    yy_scan_string(input);
    if (yyparse() != 0) return;

    print_cmd(parse_ret);
}

int main (void) {
    char *s;

    init_completion();
    init_readline();
    vars = hashmap_init();
    hashmap_add(vars, "v1", "coucou");
    // TODO: add env variables

    while((s = readline ("mpsh> "))) {
        command_line_handler(s);
        add_history(s);
        free(s);
    }
    return 0;
}
