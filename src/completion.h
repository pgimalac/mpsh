#ifndef COMPLETION
#define COMPLETION

#include "array.h"

void init_completion ();
array_t* get_all_files(char*);
char** fileman_completion (const char*, int, int);
int find_files_with_ext(char**);

#endif
