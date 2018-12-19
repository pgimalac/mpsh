#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include "parsing.h"
#include "builtin.h"

unsigned char
builtin_echo (cmd_s* cmd){
    for (int i = 1; cmd->argv[i]; i++)
        printf(cmd->argv[i + 1] ? "%s " : "%s\n", cmd->argv[i]);

    return 0;
}

unsigned char
builtin_exit (cmd_s* cmd){
    int n = 0;
    if (cmd->argv[1]){
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

unsigned char
builtin_cd (cmd_s* cmd){
    if (cmd->argv[1] == 0) return 1;
    if (chdir(cmd->argv[1]) == 0) return 0;
    return 1;
}

unsigned char
builtin_alias (cmd_s* cmd){
    return 0;
}

unsigned char
builtin_export (cmd_s* cmd){
    return 0;
}

unsigned char
builtin_unalias (cmd_s* cmd){
    return 0;
}

unsigned char
builtin_type (cmd_s* cmd){
    return 0;
}

unsigned char
builtin_umask (cmd_s* cmd){
    return 0;
}

unsigned char
builtin_history (cmd_s* cmd){
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

unsigned char
exec_builtin(cmd_s* cmd){
    for (int i = 0; builtin_names[i]; i++)
        if (strcmp(cmd->argv[0], builtin_names[i]) == 0)
            return (*(builtin_functions[i]))(cmd);

    return 2;
}
