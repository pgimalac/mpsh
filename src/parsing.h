/**
 * File managing the parsing of a command line
 */

#ifndef H_PARSER
#define H_PARSER

#include "list.h"

typedef struct var_d {
    char *name;
    char *value;
} var_d;

typedef enum {
      REDIR_WRITE,          /* > */
      REDIR_APP,            /* >> */
      REDIR_ERRWRITE,		/* &>, >& */
      REDIR_READWRITE,		/* <> */
      REDIR_READ,           /* < */
      REDIR_HEREDOC,		/* << */
      REDIR_MERGEIN,		/* <&n */
      REDIR_MERGEOUT,		/* >&n */
      REDIR_CLOSE,          /* >&-, <&- */
} redir_t;

/**
 * Redirections between file descriptors
 */
typedef struct simple_redir {
    int fd1, fd2;
    redir_t type;
} simple_redir;

/**
 * Redirection with a file
 */
typedef struct file_redir {
    int fd;
    char *fname;
    redir_t type;
} file_redir;

typedef struct redir {
    union {
        file_redir *fredir;
        simple_redir *sredir;
    };

    short is_simple;
} redir;

/**
 * Any basic command is a vector of arguments
 * and a list of redirections to apply on the new process
 */
typedef struct cmd_s {
    char **argv;

    list_t *redirs;
} cmd_s;

typedef enum
    {
     PIPE,                     /* | */
     AND,                      /* && */
     OR,                       /* || */
     SEMI                      /* ; or \n */
    } bin_op;

typedef struct cmd_t cmd_t;

typedef struct cmd_b {
    cmd_t *left, *right;

    bin_op type;
} cmd_b;

typedef enum
    {
     BIN,
     SIMPLE,
     VAR
    } cmd_type;

struct cmd_t {
  union {
    cmd_b *cmd_bin;
    cmd_s *cmd_sim;
    var_d *cmd_var;
  };

  cmd_type type;
};

/**
 * Constructors
 */

simple_redir* create_simple_redir (redir_t type, int fd1, int fd2);

file_redir* create_file_redir (redir_t type, int fd1, char *name);

redir* redir_from_simple (simple_redir *r);

redir* redir_from_file (file_redir *r);

var_d* create_var_d (char *name, char *value);

cmd_s* create_cmd_s (char **argv, list_t *redirs);

cmd_b* create_cmd_b (bin_op op, cmd_t *left, cmd_t *right);

cmd_t* create_cmd_with_simple (cmd_s *s);

cmd_t* create_cmd_with_bin_op (cmd_b *b);

cmd_t* create_cmd_with_var_def (var_d *v);

void free_var_d(var_d*);

void free_simple_redir(simple_redir*);

void free_file_redir(file_redir*);

void free_redir (redir*);

void free_cmd_s(cmd_s*);

void free_cmd_b(cmd_b*);

void free_cmd_t(cmd_t*);

#endif
