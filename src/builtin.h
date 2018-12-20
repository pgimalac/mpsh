#ifndef BUILTIN
#define BUILTIN

#include "parsing.h"

char* builtin_names[];

short is_builtin (char*);

unsigned char exec_builtin (cmd_s*);

#endif
