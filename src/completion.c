#include <dirent.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "builtin.h"
#include "completion.h"
#include "env.h"
#include "types/array.h"
#include "types/hashmap.h"
#include "utils.h"

#define BIN_SEPARATORS                                                         \
    (char *[]) { "&", "|", ";", NULL }
#define REDIR_SEPARATORS                                                       \
    (char *[]) { ">", "<", NULL }

extern hashmap_t *vars;
extern hashmap_t * compl ;

static char **completions = NULL;

static void fill_dir(char *path, short rec, array_t *arr) {
    DIR *dir = opendir(path);
    if (!dir) {
        free(path);
        return;
    }

    array_add(arr, path);
    char *ret = NULL;
    struct dirent *entry;
    while (!ret && (entry = readdir(dir)))
        if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name))
            continue;
        else if (entry->d_type == DT_REG)
            array_add(arr, strdup(entry->d_name));
        else if (rec && entry->d_type == DT_DIR)
            fill_dir(strappl(path, "/", entry->d_name, NULL), rec, arr);

    closedir(dir);
}

static array_t *get_all_files(char *var_path) {
    if (!var_path)
        return NULL;

    array_t *arr = array_init();
    if (!arr)
        return NULL;

    int l;
    short rec = 0;
    char *buff = NULL;

    for (char *path = strtok(var_path, ":"); path && !buff;
         path = strtok(NULL, ":")) {
        l = strlen(path);
        if (l >= 1 && path[l - 1] == '/') {
            path[l - 1] = '\0';
            if (l >= 2 && path[l - 2] == '/') {
                path[l - 2] = '\0';
                rec = 1;
            }
        }

        fill_dir(strdup(path), rec, arr);
    }

    return arr;
}

static void init_completion() {
    char *path = get_var("CHEMIN");
    if (!path)
        return;

    array_t *completions_array = get_all_files(path);
    free(path);

    for (char **c = builtin_names; *c; c++)
        array_add(completions_array, strdup(*c));

    array_add(completions_array, NULL);
    completions = array_to_tab(completions_array);
}

static char *command_generator(const char *com, int num) {
    static int len;
    static char **completion;

    if (num == 0) {
        len = strlen(com);
        init_completion();
        completion = completions;
    }

    while (*completion)
        if (strncmp(*completion, com, len) == 0)
            return *completion++;
        else
            free(*completion++);

    free(completions);
    return NULL;
}

static int begin_command, end_command;

int find_files_with_ext(char **str) {
    if (!str)
        return 0;
    if (begin_command == -1 || end_command == -1)
        return 0;

    char *command =
        strndup(rl_line_buffer + begin_command, end_command - begin_command);
    char *filter = hashmap_get(compl, command), *filter_cpy, *tmp;
    free(command);

    if (!filter)
        return 0;

    short string_boolean, pattern_boolean;
    int index = 1, length_string, length_pattern, i;
    for (char **string = str + 1; *string; string++) {
        length_string = strlen(*string);
        tmp = strappl("./", *string, NULL);
        string_boolean = is_valid_dir_path(tmp);
        free(tmp);
        filter_cpy = strdup(filter);
        for (char *pattern = strtok(filter_cpy, ":");
             !string_boolean && pattern; pattern = strtok(NULL, ":")) {
            length_pattern = strlen(pattern);
            if (length_pattern < length_string) {
                for (i = 1, pattern_boolean = 1;
                     pattern_boolean && i <= length_pattern; i++)
                    pattern_boolean &= pattern[length_pattern - i] ==
                                       (*string)[length_string - i];
                if (pattern_boolean)
                    pattern_boolean = (*string)[length_string - i] == '.';
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

static void find_command(char *str) {
    char *s = find_last_str(str, BIN_SEPARATORS),
         *tmp = find_last_str(str, REDIR_SEPARATORS);

    if (tmp > s)
        return;

    if (!s)
        s = str;
    else
        s = strpbrk(s, " \t");
    if (!s)
        return;

    s += strspn(s, " \t");

    begin_command = s - str;
    tmp = strchr(s, ' ');
    if (tmp)
        end_command = tmp - str;
    else
        end_command = strlen(str);
}

char **fileman_completion(const char *com, int start, int end) {
    begin_command = end_command = -1;
    char *st = strndup(rl_line_buffer, end + 1);
    find_command(st);
    free(st);

    if (start == begin_command)
        return rl_completion_matches(com, command_generator);
    return NULL;
}
