#include <stdio.h>
#include <string.h>

#include "env.h"

extern char **environ;

void
init_env_variables(hashmap_t *map) {
    char *name, *value;
    char **envp = environ;

    while (*envp) {
        value = strdup(*envp++);
        name = strsep(&value, "=");
        hashmap_add(map, name, value, 1);
    }
}
