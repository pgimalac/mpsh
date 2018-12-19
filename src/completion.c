#include "completion.h"

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

char *dupstr(char *s){
    char *r = malloc(strlen(s) + 1);
    strcpy (r, s);
    return r;
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



