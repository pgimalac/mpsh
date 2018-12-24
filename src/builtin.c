#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <readline/history.h>

#include "hashmap.h"
#include "command.h"
#include "builtin.h"

extern hashmap_t *aliases;

/**
 * echo $var :
 * affiche la valeur de la variable var
 */
unsigned char builtin_echo (cmd_s* cmd){
    for (int i = 1; cmd->argv[i]; i++)
        printf(cmd->argv[i + 1] ? "%s " : "%s\n", cmd->argv[i]);
    printf("\n");

    return 0;
}

/**
 * exit [n] :
 * permet de sortir du shell avec la valeur de retour n si n est spécifié,
 * la valeur de retour de la dernière commande lancée sinon
 */
unsigned char builtin_exit (cmd_s* cmd){
    int n = 0;
    if (cmd->argv[1]){
        if (cmd->argv[2]){
            fprintf(stderr, "%s\n", "exit: too many arguments.");
            return 1;
        }

        for (int i = 0; cmd->argv[1][i]; i++)
            if (cmd->argv[1][i] >= '0' && cmd->argv[1][i] <= '9'){
                n *= 10;
                n += cmd->argv[1][i] - '0';
            } else{
                fprintf(stderr, "%s\n", "exit: invalid argument.");
                return 1;
            }
    }

    if (n >= 0 && n < 256)
        exit(n);

    fprintf(stderr, "%s\n", "exit: the return status must be between 0 and 255.");
    return 1;
}

/**
 * cd [dir] :
 * change le répertoire courant
 */
unsigned char builtin_cd (cmd_s* cmd){
    if (!cmd->argv[1]) return 0;

    if (chdir(cmd->argv[1])){
        perror("cd");
        return 1;
    }

    return 0;
}

static void print_alias (char* k, char* v){
    printf("%s=%s\n", k, v);
}

/**
 * alias [name=value] :
 * affiche les alias ou met en place un alias
 */
unsigned char builtin_alias (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0] || strcmp("alias", cmd->argv[0]))
        return 1;

    if (!cmd->argv[1]){ // no argument, print all aliases
        hashmap_iterate(aliases, print_alias);
        return 0;
    }

    unsigned char ret = 0;
    char* tmp;
    for (char** st = cmd->argv + 1; *st; st++){
        int p = -1;
        for (int i = 0; (*st)[i]; i++)
            if ((*st)[p] == '='){
                p = i;
                break;
            }

        if (p == 0){
            fprintf(stderr, "mpsh: %s not found\n", *st + 1);
            return 1;
        }
        if (p == -1){
            tmp = hashmap_get(aliases, *st);
            if (tmp)
                print_alias(*st, tmp);
            else
                ret = 1;
            continue;
        }

        char* k = strndup(*st, p);
        char* v = strdup(*st + p + 1);

        hashmap_add(aliases, k, v, 1);
    }

    return ret;
}

/**
 * export var[=word] :
 * exporte une variable ( i.e. la transforme en variable d'environnement)
 */
unsigned char builtin_export (cmd_s* cmd){
    return 0;
}

/**
 * unalias name : supprime un alias
 */
unsigned char builtin_unalias (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0] || strcmp("unalias", cmd->argv[0]))
        return 1;

    if (!cmd->argv[1]){
        fprintf(stderr, "%s\n", "unalias: not enough arguments");
        return 1;
    }

    unsigned char ret = 0;
    for (char** st = cmd->argv + 1; *st; st++)
        if (!hashmap_remove(aliases, *st, 1)){
            fprintf(stderr, "unalias: no such alias: %s\n", *st);
            ret = 1;
        }
    return ret;
}

/**
 * type name [name ...] :
 * indique comment chaque nom est interprété
 * (comme alias, commande interne ou commande externe)
 * s'il est utilisé pour lancer une commande
 */
unsigned char builtin_type (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0] || strcmp("type", cmd->argv[0]) || !cmd->argv[1])
        return 1;

    unsigned char ret = 0;
    for (char** st = cmd->argv + 1; *st; st++)
        if (hashmap_contains(aliases, *st))
            printf("%s is an alias for %s\n", *st, hashmap_get(aliases, *st));
        else if (is_builtin(*st))
            printf("%s is a mpsh builtin\n", *st);
        else {
            char* path = find_cmd(*st);
            if (path){
                printf("%s\n", path);
                free(path);
            } else {
                ret = 1;
                printf("%s not found\n", *st);
            }
        }


    return ret;
}

/**
 * Bonus builtin : which (because of PATH / CHEMIN)
 * (almost the same as type)
 */
unsigned char builtin_which (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0] || strcmp("which", cmd->argv[0]) || !cmd->argv[1])
        return 1;

    unsigned char ret = 0;
    for (char** st = cmd->argv + 1; *st; st++)
        if (hashmap_contains(aliases, *st))
            printf("%s: aliased to %s\n", *st, hashmap_get(aliases, *st));
        else if (is_builtin(*st))
            printf("%s: mpsh builtin command\n", *st);
        else {
            char* path = find_cmd(*st);
            if (path){
                printf("%s\n", path);
                free(path);
            } else {
                ret = 1;
                printf("%s not found\n", *st);
            }
        }

    return ret;
}

/**
 * umask mode : met en place un masque pour les droits
 */
unsigned char builtin_umask (cmd_s* cmd){
    return 0;
}

static int log_10 (int n) {
    int l = 0;
    while (n > 0) {
        n /= 10;
        l++;
    }

    return l;
}

static short is_number (char *c) {
    if (!isdigit(*c) && *c != '-') return 0;
    c++;
    while (isdigit(*c)) c++;
    return *c == 0;
}

/**
 * history [n] :
 * - sans argument, affiche la liste numérotée de l'historique des commandes,
 * - avec un argument `n` entier positif, relance la commande
 *   dont le numéro dans la liste est `n`,
 * - avec un argument `-n` entier négatif,
 *   fixe à `n` le nombre de commandes enregistrées dans l'historique.
 */
unsigned char builtin_history (cmd_s* cmd) {
    if (cmd->argv[1]) {
        if (!is_number(cmd->argv[1])) {
            fprintf (stderr, "not a number\n");
            return 1;
        }

        int n = atoi(cmd->argv[1]);
        if (n > history_length || n == 0) {
            fprintf(stderr, "invalid argument %d\n", n);
            return 1;
        }

        if (n < 0) {
            stifle_history(-n + 1);
            return 0;
        }

        command_line_handler(history_get(n + history_base)->line);
        return 0;
    }

    int margin = log_10(history_length);
    for (int i = 1; i < history_length; i++)
        printf("%*d %s\n", margin, i, history_get(i + history_base)->line);
    return 0;
}


char* builtin_names [] = {"cd", "echo", "alias",
                          "exit", "export", "unalias",
                          "type", "umask", "history",
                          "which", NULL};

typedef unsigned char (*builtin)(cmd_s*);

builtin builtin_functions[] = {builtin_cd, builtin_echo, builtin_alias,
                               builtin_exit, builtin_export, builtin_unalias,
                               builtin_type, builtin_umask, builtin_history,
                               builtin_which, NULL};

short is_builtin (char* s){
    for (int i = 0; builtin_names[i]; i++)
        if (strcmp(s, builtin_names[i]) == 0)
            return 1;

    return 0;
}

unsigned char exec_builtin(cmd_s* cmd){
    for (int i = 0; builtin_names[i]; i++)
        if (strcmp(cmd->argv[0], builtin_names[i]) == 0)
            return (*(builtin_functions[i]))(cmd);

    return 1;
}
