#ifndef COMPLETION
#define COMPLETION

#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>

#include "array.h"

void init_completion ();
char** fileman_completion (const char*, int, int);

#endif
