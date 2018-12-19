#ifndef BUILTIN
#define BUILTIN

#include <stddef.h>
#include <string.h>
#include "parsing.h"

short is_built_in (char*);
char exec_builtin (cmd_s*);

char builtin_cd (cmd_s*);
char builtin_echo (cmd_s*);
char builtin_alias (cmd_s*);
char builtin_exit (cmd_s*);
char builtin_export (cmd_s*);
char builtin_unalias (cmd_s*);
char builtin_type (cmd_s*);
char builtin_umask (cmd_s*);
char builtin_history (cmd_s*);

#endif
