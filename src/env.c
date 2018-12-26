#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "env.h"
#include "hashmap.h"

extern char **environ;
extern hashmap_t* vars;

static char* env_var (char* k, char* v){
    char* str = malloc(sizeof(char) * (strlen(k) + strlen(v) + 2));
    sprintf(str, "%s=%s", k, v);
    return str;
}

/**
 * Return the value of the variable k. Obtained with malloc.
 */
char* get_var (char* k){
    if (!k)
        return NULL;

    char* ret;
    if ((ret = getenv(k)) || (ret = hashmap_get(vars, k)))
        return strdup(ret);
    return NULL;
}

char* secure_get_var(char* k){
    char* s = get_var(k);
    if (s)
        return s;
    return strdup("");
}

/**
 * Takes a key, a value, a short
 * Both the key and the value must be free-able and not touched after given to this function
 * If the short is 0, then the variable is added as what it was (or local variable if it wasn't set)
 * If the short is 1, the variable is added as an environnement variable (possibly removed from local variables)
 * If the value is NULL, then it is replaced with either the former value (if the variable already exists) or the empty string
 */
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
    for (char** envp = environ; *envp; envp ++)
        *envp = strdup(*envp);
}
