#include "builtin.h"

char* builtin_names [] = {"cd", "echo", "alias",
                          "exit", "export", "unalias",
                          "type", "umask", "history",
                          NULL};

char(*builtin_functions[])(cmd_s*) = {builtin_cd, builtin_echo, builtin_alias,
                                      builtin_exit, builtin_export, builtin_unalias,
                                      builtin_type, builtin_umask, builtin_history,
                                      NULL};

short is_built_in (char* s){
    if (s == NULL)
        return 0;

    for (char* c = builtin_names[0]; c; c++)
        if (strcmp(s, c) == 0)
            return 1;

    return 0;
}

char exec_builtin(cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;

    for (int i = 0; builtin_names[i]; i++)
        if (strcmp(cmd->argv[i], builtin_names[i]) == i)
            return (*builtin_functions[i])(cmd);

    return 2;
}


char builtin_echo (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;

    for (char* c = cmd->argv[1]; c; c++)
        printf("%s\n", c);

    return 0;
}

char builtin_exit (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;

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

char builtin_cd (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;
    return 0;
}

char builtin_alias (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;
    return 0;
}

char builtin_export (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;
    return 0;
}

char builtin_unalias (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;
    return 0;
}

char builtin_type (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;
    return 0;
}

char builtin_umask (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;
    return 0;
}

char builtin_history (cmd_s* cmd){
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 1;
    return 0;
}

