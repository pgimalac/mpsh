#ifndef COMMAND
#define COMMAND

#include "hashmap.h"
#include "parsing.h"

extern int yy_scan_string(const char *);
extern cmd_t *parse_ret;
extern hashmap_t *vars;

/**
 * Handler called when a child terminate
 * Used if the process was running in background
 */
void sigchild_handler(int);

/**
 * Helper.
 * Print the syntaxic tree of the command
 */
void print_cmd (cmd_t*);

/**
 * Execute the given command and return his exit code
 */
unsigned char exec_cmd (cmd_t*);

/**
 * Parse and execute the given instruction
 */
void command_line_handler (char*);

/**
 * Test if the given string is a valid command
 */
short is_cmd(char*);

/**
 * Return the path to the command if its a valid one
 * Return NULL otherwise
 */
char* find_cmd(char*);

#endif
