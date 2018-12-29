#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "array.h"
#include "builtin.h"
#include "completion.h"
#include "hashmap.h"
#include "env.h"
#include "utils.h"

extern hashmap_t* vars;
extern hashmap_t* compl;

static char **completions;
static int nb_completions;

static int fill_with_dir (char *path, array_t *array) {
    char *buf;
    struct dirent *entry;
    struct stat stat_buf;
    DIR *dir = opendir(path);

    if (!dir) return -1;
    while ((entry = readdir(dir)))
        if (entry->d_type == DT_REG) {
            int s = strlen(path) + strlen(entry->d_name) + 2;
            buf = malloc(sizeof(char) * s);
            sprintf(buf, "%s/%s", path, entry->d_name);

            if(stat(buf, &stat_buf) == 0 &&
               stat_buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {

                buf = malloc(sizeof(char) * 256);
                strncpy(buf, entry->d_name, 255);
                array_add(array, buf);
            }
        }

    closedir(dir);
    return 0;
}

array_t* get_all_files (char *var_path) {
    char *path, *name, *tmp;
    char *saveptr1, *saveptr2;
    struct stat stat_buf;

    array_t *array = array_init();

    for (path = strtok_r(var_path, ":", &saveptr1); path; path = strtok_r(0, ":", &saveptr1)) {
        if (stat(path, &stat_buf) == 0) {
            if (S_ISDIR(stat_buf.st_mode)) {
                if (fill_with_dir(path, array) == -1)
                    fprintf(stderr, "Can't open directory %s\n", path);
            } else if (S_ISREG(stat_buf.st_mode) &&
                       stat_buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                for (tmp = strtok_r(path, "/", &saveptr2); tmp; path = 0,
                         tmp = strtok_r(0, "/", &saveptr2))
                    name = tmp;
                array_add(array, name);
            }
        } else fprintf(stderr, "Can't open %s\n", path);
    }

    return array;
}

void init_completion () {
    char *path = get_var("CHEMIN");
    if (!path)
        return;

    array_t *completions_array = get_all_files(path);
    free(path);

    for (char **c = builtin_names; *c; c++)
        array_add(completions_array, *c);

    nb_completions = completions_array->size;
    completions = array_to_tab(completions_array);
}

static char *command_generator (const char *com, int num){
    static int indice, len;
    char *completion;

    if (num == 0){
        indice = 0;
        len = strlen(com);
    }

    while (indice < nb_completions) {
        completion = completions[indice++];

        if (strncmp (completion, com, len) == 0)
            return strdup(completion);
    }

    return NULL;
}

char** matches = NULL;
char* command = NULL;

int find_files_with_ext(char** str){
    if (!str)
        return 0;
    if (!command)
        return 0;

    char* filter = hashmap_get(compl, command), *filter_cpy;

    if (!filter)
        return 0;

    short string_boolean, pattern_boolean;
    int index = 1, length_string, length_pattern, i;
    for (char** string = str + 1; *string; string++){
        length_string = strlen(*string);
        string_boolean = (*string)[length_string - 1] == '/';
        filter_cpy = strdup(filter);
        for (char* pattern = strtok(filter_cpy, ":"); !string_boolean && pattern; pattern = strtok(NULL, ":")){
            length_pattern = strlen(pattern);
            if (length_pattern < length_string){
                for (i = 1, pattern_boolean = 1; pattern_boolean && i <= length_pattern; i++)
                    pattern_boolean &= pattern[length_pattern - i] == (*string)[length_string - i];
                if (pattern_boolean) pattern_boolean = (*string)[length_string - i] == '.';
                string_boolean |= pattern_boolean;
            }
        }
        free(filter_cpy);
        if (string_boolean)
            str[index++] = *string;
        else
            free(*string);
    }
    str[index] = NULL;
    return 0;
}


static char* find_command(char* str){
    char* separators[] = {"&", "||", ";", ">", "<", NULL}, *tmp;

    char* s = find_last_str(str, separators);
    if (!s)
        s = str;
    else
        s = strpbrk(s, " ");
    if (!s) return NULL;

    s += strspn(s, " ");
    if (!*s) return NULL;

    int l = strchr(s, ' ') - s;
    tmp = malloc(sizeof(char) * (l + 1));
    strncpy(tmp, s, l);
    tmp[l] = '\0';
    return tmp;
}

char ** fileman_completion (const char *com, int start, int end) {
    if (matches){
        for (char** st = matches; *st; st++)
            free(*st);
        free(matches);
    }

    char* st = strdup(rl_line_buffer);
    st[end + 1] = '\0';
    free(command);
    command = find_command(st);
    free(st);

    if (start == 0)
        matches = rl_completion_matches (com, command_generator);
    else
        matches = NULL;

    return matches;
}
