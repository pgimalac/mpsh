#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <readline/history.h>

#include "builtin.h"

// echo $var : affiche la valeur de la variable var
unsigned char builtin_echo (cmd_s* cmd){
    for (int i = 1; cmd->argv[i]; i++)
        printf(cmd->argv[i + 1] ? "%s " : "%s\n", cmd->argv[i]);

    return 0;
}

// exit [n] : permet de sortir du shell avec la valeur de retour n si n est spécifié, la valeur de retour de la dernière commande lancée sinon
unsigned char builtin_exit (cmd_s* cmd){
    int n = 0;
    if (cmd->argv[1]){
        if (cmd->argv[2]){
            fprintf(stderr, "%s\n", "Too many arguments.");
            return 1;
        }

        for (int i = 0; cmd->argv[1][i]; i++)
            if (cmd->argv[1][i] >= '0' && cmd->argv[1][i] <= '9'){
                n *= 10;
                n += cmd->argv[1][i] - '0';
            } else{
                fprintf(stderr, "%s\n", "Invalid argument.");
                return 1;
            }
    }

    if (n >= 0 && n < 256)
        exit(n);

    fprintf(stderr, "%s\n", "The return status must be between 0 and 255.");
    return 1;
}

// cd [dir] : change le répertoire courant
unsigned char builtin_cd (cmd_s* cmd){
    if (!cmd->argv[1]) return 0;

    if (chdir(cmd->argv[1])){
        perror("cd");
        return 1;
    }

    return 0;
}

// alias [name=value] : affiche les alias ou met en place un alias
unsigned char builtin_alias (cmd_s* cmd){
    return 0;
}

// export var[=word] : exporte une variable ( i.e. la transforme en variable d'environnement)
unsigned char builtin_export (cmd_s* cmd){
    return 0;
}

// unalias name : supprime un alias
unsigned char builtin_unalias (cmd_s* cmd){
    return 0;
}

// type name [name ...] : indique comment chaque nom est interprété (comme alias, commande interne ou commande externe) s'il est utilisé pour lancer une commande
unsigned char builtin_type (cmd_s* cmd){
    return 0;
}

// umask mode : met en place un masque pour les droits
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
    while (isdigit(*c)) c++;
    return *c == 0;
}

// history [n] :
// sans argument, affiche la liste numérotée de l'historique des commandes,
// avec un argument n entier positif, relance la commande dont le numéro dans la liste est n,
// avec un argument -n entier négatif, fixe à n le nombre de commandes enregistrées dans l'historique.
unsigned char builtin_history (cmd_s* cmd){
    int len = where_history();
    if (cmd->argv[1] != 0) {
        if (!is_number(cmd->argv[1])) {
            fprintf (stderr, "not a number\n");
            return 1;
        }
        int n = atoi(cmd->argv[1]);
        if (n > len) {
            fprintf(stderr, "too large number\n");
            return 1;
        }

        printf("%s\n", history_get(n)->line);
        return 0;
    }

    int margin = log_10(len);
    for (int i = 1; i <= len; i++)
        printf("%*d %s\n", margin, i, history_get(i)->line);
    return 0;
}

char* builtin_names [] = {"cd", "echo", "alias",
                          "exit", "export", "unalias",
                          "type", "umask", "history",
                          NULL};

typedef unsigned char (*builtin)(cmd_s*);

builtin builtin_functions[] = {builtin_cd, builtin_echo, builtin_alias,
                               builtin_exit, builtin_export, builtin_unalias,
                               builtin_type, builtin_umask, builtin_history,
                               NULL};

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
