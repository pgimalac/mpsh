#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "completion.h"

static char **completions;
static int nb_completions;

int fill_with_dir (char *path, array_t *array) {
    DIR *dir;
    char *buf;
    struct dirent *entry;
    struct stat stat_buf;

    if ((dir = opendir(path)) == 0) return -1;
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

    for (path = strtok_r(var_path, ":", &saveptr1); path; var_path = 0,
             path = strtok_r(0, ":", &saveptr1)) {
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
    char *path = strdup(getenv("PATH"));
    array_t *completions_array = get_all_files(path);
    nb_completions = completions_array->size;
    completions = array_to_tab(completions_array);
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
            return strdup(completion);
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
