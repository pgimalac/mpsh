#ifndef BUILTIN
#define BUILTIN

#include "parsing.h"

/**
 * NULL-terminated list of all builtins names
 */
extern char* builtin_names[];

/**
 * Test if the given string is a builtin
 */
short is_builtin (char*);

/**
 * Execute the given command
 * Assume it's a builtin
 * Nexts parameters are file descriptors which refers
 * (in this order) to standard input, output and error output
 */
unsigned char exec_builtin (cmd_s*, int in, int out, int err);

#endif
