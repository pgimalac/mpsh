#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <readline/history.h>

#include "hashmap.h"
#include "command.h"
#include "builtin.h"
#include "env.h"
#include "utils.h"
#include "array.h"

extern hashmap_t *aliases;
extern hashmap_t *compl;
extern char** environ;
extern void exit_mpsh(int);

/**
 * echo $var :
 * affiche la valeur de la variable var
 */
unsigned char builtin_echo (cmd_s* cmd, int fdin, int fdout, int fderr){
    for (int i = 1; cmd->argv[i]; i++)
        dprintf(fdout, cmd->argv[i + 1] ? "%s " : "%s\n", cmd->argv[i]);

    return 0;
}

/**
 * exit [n] :
 * permet de sortir du shell avec la valeur de retour n si n est spécifié,
 * la valeur de retour de la dernière commande lancée sinon
 */
unsigned char builtin_exit (cmd_s* cmd, int fdin, int fdout, int fderr){
    int n = 0;
    if (cmd->argv[1]){
        if (cmd->argv[2]){
            dprintf(fderr, "%s\n", "exit: too many arguments.");
            return 1;
        }

        for (int i = 0; cmd->argv[1][i]; i++)
            if (cmd->argv[1][i] >= '0' && cmd->argv[1][i] <= '9'){
                n *= 10;
                n += cmd->argv[1][i] - '0';
            } else{
                dprintf(fderr, "%s\n", "exit: invalid argument.");
                return 1;
            }
    }

    if (n >= 0 && n < 256)
        exit_mpsh(n);

    dprintf(fderr, "%s\n", "exit: the return status must be between 0 and 255.");
    return 1;
}

/**
 * cd [dir] :
 * change le répertoire courant
 */
unsigned char builtin_cd (cmd_s* cmd, int fdin, int fdout, int fderr){
    if (!cmd || !cmd->argv || !cmd->argv[1]) return 0;

    if (chdir(cmd->argv[1])){
        perror("cd");
        return 1;
    }

    char* st = getcwd(0, 0);
    if (st)
        add_var(strdup("PWD"), st, 0);

    return 0;
}

static void print_alias (int fdout, char* k, char* v){
    dprintf(fdout, "%s=%s\n", k, v);
}

/**
 * alias [name=value] :
 * affiche les alias ou met en place un alias
 */
unsigned char builtin_alias (cmd_s* cmd, int fdin, int fdout, int fderr){
    if (!cmd->argv[1]){ // no argument, print all aliases
        hashmap_iterate(aliases, fdout, print_alias);
        return 0;
    }

    unsigned char ret = 0;
    for (char** st = cmd->argv + 1; *st; st++){
        char *name, *value, *tmp;
        tmp = strdup(*st);
        name = strsep(&tmp, "=");
        if (tmp == 0) {
            value = hashmap_get(aliases, name);
            if (value) print_alias(fdout, name, value);
            else ret = 1;
        } else hashmap_add(aliases, name, strdup(tmp), 1);
    }

    return ret;
}

/**
 * export var[=word] :
 * exporte une variable ( i.e. la transforme en variable d'environnement)
 */
unsigned char builtin_export (cmd_s* cmd, int fdin, int fdout, int fderr){
    if (!cmd->argv[1]){
        for (char** st = environ; *st; st++)
            dprintf(fdin, "%s\n", *st);
        return 0;
    }

    for (char** st = cmd->argv; *st; st++){
        short s = -1;
        for (int i = 0; (*st)[i]; i++)
            if ((*st)[i] == '='){
                s = i;
                break;
            }

        if (s == -1)
            add_var(*st, NULL, 1);
        else {
            (*st)[s] = '\0';
            add_var(*st, strdup(*st + s + 1), 1);
        }
    }
    return 0;
}

/**
 * unalias name : supprime un alias
 */
unsigned char builtin_unalias (cmd_s* cmd, int fdin, int fdout, int fderr){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;

    if (!cmd->argv[1]){
        dprintf(fderr, "%s\n", "unalias: not enough arguments");
        return 1;
    }

    unsigned char ret = 0;
    for (char** st = cmd->argv + 1; *st; st++)
        if (!hashmap_remove(aliases, *st, 1)){
            dprintf(fderr, "unalias: no such alias: %s\n", *st);
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

unsigned char builtin_type (cmd_s* cmd, int fdin, int fdout, int fderr){
    if (!cmd || !cmd->argv || !cmd->argv[0] || !cmd->argv[1])
        return 1;

    unsigned char ret = 0;
    for (char** st = cmd->argv + 1; *st; st++)
        if (hashmap_contains(aliases, *st))
            dprintf(fdout, "%s is an alias for %s\n", *st, hashmap_get(aliases, *st));
        else if (is_builtin(*st))
            dprintf(fdout, "%s is a mpsh builtin\n", *st);
        else {
            char* path = find_cmd(*st);
            if (path){
                dprintf(fdout, "%s\n", path);
                free(path);
            } else {
                ret = 1;
                dprintf(fderr, "%s not found\n", *st);
            }
        }

    return ret;
}

/**
 * Bonus builtin : which (because of PATH / CHEMIN)
 * (almost the same as type)
 */

unsigned char builtin_which (cmd_s* cmd, int fdin, int fdout, int fderr){
    if (!cmd || !cmd->argv || !cmd->argv[0] || !cmd->argv[1])
        return 1;

    unsigned char ret = 0;
    for (char** st = cmd->argv + 1; *st; st++)
        if (hashmap_contains(aliases, *st))
            dprintf(fdout, "%s: aliased to %s\n", *st, hashmap_get(aliases, *st));
        else if (is_builtin(*st))
            dprintf(fdout, "%s: mpsh builtin command\n", *st);
        else {
            char* path = find_cmd(*st);
            if (path){
                dprintf(fdout, "%s\n", path);
                free(path);
            } else {
                ret = 1;
                dprintf(fderr, "%s not found\n", *st);
            }
        }

    return ret;
}

/**
 * umask mode : met en place un masque pour les droits
 */
unsigned char builtin_umask (cmd_s* cmd, int fdin, int fdout, int fderr){
    mode_t mask;

    if (!cmd->argv[1]) {
        mask = umask(0);
        printf("%03o\n", mask);
        return 0;
    }

    if (!is_positive_number(cmd->argv[1])) {
        dprintf(fderr, "umask: bad symbol %s\n", cmd->argv[1]);
        return 1;
    }

    mask = strtol(cmd->argv[1], 0, 8);
    umask(mask);
    return 0;
}

/**
 * history [n] :
 * - sans argument, affiche la liste numérotée de l'historique des commandes,
 * - avec un argument `n` entier positif, relance la commande
 *   dont le numéro dans la liste est `n`,
 * - avec un argument `-n` entier négatif,
 *   fixe à `n` le nombre de commandes enregistrées dans l'historique.
 */
unsigned char builtin_history (cmd_s* cmd, int fdin, int fdout, int fderr) {
    if (cmd->argv[1]) {
        if (!is_number(cmd->argv[1])) {
            dprintf (fderr, "history: not a number\n");
            return 1;
        }

        int n = atoi(cmd->argv[1]);
        if (n > history_length || n == 0) {
            dprintf(fderr, "history: invalid argument %d\n", n);
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
        dprintf(fdout, "  %*d %s\n", margin, i, history_get(i + history_base)->line);
    return 0;
}

unsigned char builtin_complete(cmd_s* cmd, int fdin, int fdout, int fderr){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;

    if (!cmd->argv[1]){
        hashmap_print(compl, fdin);
        return 0;
    }

    if (!cmd->argv[2]){
        hashmap_remove(compl, cmd->argv[1], 1);
        return 0;
    }

    array_t* arr = array_init();
    for (char** st = cmd->argv + 2; *st; st++){
        array_add(arr, *st);
        array_add(arr, ":");
    }
    array_add(arr, NULL);
    char** st = array_to_tab(arr);
    char* buff = strappv(st);

    hashmap_add(compl, strdup(cmd->argv[1]), buff, 1);

    return 0;
}

char* builtin_names [] = {"cd", "echo", "alias",
                          "exit", "export", "unalias",
                          "type", "umask", "history",
                          "which", "complete", NULL};

typedef unsigned char (*builtin)(cmd_s*, int fdin, int fdout, int fderr);

builtin builtin_functions[] = {builtin_cd, builtin_echo, builtin_alias,
                               builtin_exit, builtin_export, builtin_unalias,
                               builtin_type, builtin_umask, builtin_history,
                               builtin_which, builtin_complete, NULL};

short is_builtin (char* s){
    for (int i = 0; builtin_names[i]; i++)
        if (strcmp(s, builtin_names[i]) == 0)
            return 1;

    return 0;
}

unsigned char exec_builtin(cmd_s* cmd, int fdin, int fdout, int fderr){
    for (int i = 0; builtin_names[i]; i++)
        if (strcmp(cmd->argv[0], builtin_names[i]) == 0)
            return (*(builtin_functions[i]))(cmd, fdin, fdout, fderr);

    return 1;
}
