#ifndef COMMAND
#define COMMAND

#include "hashmap.h"
#include "parsing.h"

extern int yy_scan_string(const char *);
extern cmd_t *parse_ret;
extern hashmap_t *vars;

void sigchild_handler(int);

void print_cmd (cmd_t*);
unsigned char exec_cmd (cmd_t*);

void command_line_handler (char*);

short is_cmd(char*);
char* find_cmd(char*);

#endif
