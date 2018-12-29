#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "env.h"
#include "hashmap.h"
#include "utils.h"

extern char **environ;
extern hashmap_t* vars;

static char* env_var (char* k, char* v){
    char* str = malloc(sizeof(char) * (strlen(k) + strlen(v) + 2));
    sprintf(str, "%s=%s", k, v);
    return str;
}

char* get_var (char* k){
    if (!k)
        return NULL;

    char* ret;
    if ((ret = getenv(k)) || (ret = hashmap_get(vars, k)))
        return replace_macros(ret);
    return NULL;
}

char* secure_get_var(char* k){
    char* s = get_var(k);
    if (s)
        return s;
    return strdup("");
}

void add_var (char* k, char* v, short env){
    if (!k)
        return;

    if (!v && !(v = getenv(k)) && !(v = hashmap_get(vars, k)))
        v = strdup("");

    char* str = getenv(k);
    if (str) {
        str -= 1 + strlen(k);
        env = 1;
    }
    if (hashmap_get(vars, k))
        hashmap_remove(vars, k, 1);

    if (env){
        putenv(env_var(k, v));
        free(k);
        free(v);
    } else
        hashmap_add(vars, k, v, 1);
    free(str);
}

void init_env_variables() {
    for (char** envp = environ; *envp; envp++)
        *envp = strdup(*envp);
}
