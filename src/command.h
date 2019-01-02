#ifndef COMMAND
#define COMMAND

#include "types/hashmap.h"
#include "parsing.h"

extern void* yy_scan_string(const char *);
extern cmd_t *parse_ret;
extern hashmap_t *vars;

list_t *bgps;
/**
 * Handler called when a child terminate
 * Used if the process was running in background
 */
void sigchild_handler(int);

/**
 * Helper.
 * Prints the syntaxic tree of the command
 */
void print_cmd (cmd_t*);

/**
 * Executes the given command and returns his exit code
 */
unsigned char exec_cmd (cmd_t*);

/**
 * Parses and executes the given instruction
 */
void command_line_handler (char*);

/**
 * Returns if the given string is a valid command
 */
short is_cmd(char*);

/**
 * Returns the path to the command if it is a valid one
 * Returns NULL otherwise
 */
char* find_cmd(char*);

/**
 * Executes the indicated script
 */
unsigned char exec_script(int);

#endif
