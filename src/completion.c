#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "types/array.h"
#include "builtin.h"
#include "completion.h"
#include "types/hashmap.h"
#include "env.h"
#include "utils.h"

extern hashmap_t* vars;
extern hashmap_t* compl;

char **completions = NULL;
static int nb_completions = 0;

static void fill_dir(char* path, short rec, array_t* arr) {
    DIR* dir = opendir(path);
    if (!dir) return;

    char* ret = NULL;
    struct dirent *entry;
    while (!ret && (entry = readdir(dir)))
        if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name))
            continue;
        else if (entry->d_type == DT_REG)
            array_add(arr, strdup(entry->d_name));
        else if (rec && entry->d_type == DT_DIR)
            fill_dir(strappl(path, "/", entry->d_name, NULL), rec, arr);

    free(path);
    closedir(dir);
}

static array_t* get_all_files (char *var_path) {
    if (!var_path)
        return NULL;

    array_t* arr = array_init();
    if (!arr)
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

        fill_dir(strdup(path), rec, arr);
    }

    return arr;
}

static void init_completion () {
    char *path = get_var("CHEMIN");
    if (!path)
        return;

    array_t *completions_array = get_all_files(path);
    free(path);

    for (char **c = builtin_names; *c; c++)
        array_add(completions_array, *c);

    nb_completions = completions_array->size;
/*    if (completions){
        for (char** st = completions; *st; st++)
            free(*st);
        free(completions);
    } */
    completions = array_to_tab(completions_array);
}

static char *command_generator (const char *com, int num){
    static int len;
    static char** completion;

    if (num == 0){
        len = strlen(com);
        init_completion();
        completion = completions;
    }

    while (*completion){
        if (strncmp (*completion, com, len) == 0)
            return strdup(*completion++);
        else
            completion++;
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

    char* filter = hashmap_get(compl, command), *filter_cpy, *tmp;

    if (!filter)
        return 0;

    short string_boolean, pattern_boolean;
    int index = 1, length_string, length_pattern, i;
    for (char** string = str + 1; *string; string++){
        length_string = strlen(*string);
        tmp = strappl("./", *string, NULL);
        string_boolean = is_valid_dir_path(tmp);
        free(tmp);
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
        s = strpbrk(s, " \t");
    if (!s) return NULL;

    s += strspn(s, " \t");
    if (!*s) return NULL;

    int l = strchr(s, ' ') - s;
    tmp = calloc(l + 1, sizeof(char));
    strncpy(tmp, s, l);
    return tmp;
}

char ** fileman_completion (const char *com, int start, int end) {
/*    if (matches){
        for (char** st = matches; *st; st++)
            free(*st);
        free(matches);
    }
*/
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
