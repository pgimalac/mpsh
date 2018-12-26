#ifndef BUILTIN
#define BUILTIN

#include "parsing.h"

extern char* builtin_names[];

short is_builtin (char*);
unsigned char exec_builtin (cmd_s*, int, int, int);

#endif
