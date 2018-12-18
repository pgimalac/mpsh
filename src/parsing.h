/**
 * File managing the parsing of a command line
 */

#ifndef H_PARSER
#define H_PARSER

#include "list.h"

struct var_d {
    char *name;
    char *value;
};

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
struct simple_redir {
    int fd1, fd2;
    redir_t type;
};

/**
 * Redirection with a file
 */
struct file_redir {
    int fd;
    char *fname;
    redir_t type;
};

struct redir {
    union {
        struct file_redir *fredir;
        struct simple_redir *sredir;
    };

    short is_simple;
};

/**
 * Any basic command is a vector of arguments
 * and a list of redirections to apply on the new process
 */
struct cmd_s {
    int argc;
    char **argv;

    list_t *redirs;
};

typedef enum
    {
     PIPE,                     /* | */
     AND,                      /* && */
     OR,                       /* || */
     SEMI                      /* ; or \n */
    } bin_op;

struct cmd_b {
    struct cmd *left, *right;

    bin_op type;
};

typedef enum
    {
     BIN,
     SIMPLE,
     VAR
    } cmd_type;

typedef struct cmd {
  union {
    struct var_d *cmd_v;
    struct cmd_s *cmd_s;
    struct cmd_b *cmd_b;
  };

  cmd_type type;
} cmd_t;

/**
 * Constructors
 */

struct simple_redir *
create_simple_redir (redir_t type, int fd1, int fd2);

struct file_redir *
create_file_redir (redir_t type, int fd1, char *name);

struct redir *
redir_from_simple (struct simple_redir *r);

struct redir *
redir_from_file (struct file_redir *r);

struct var_d *
create_var_d (char *name, char *value);

struct cmd_s *
create_cmd_s (int argc, char **argv, list_t *redirs);

struct cmd_b *
create_cmd_b (bin_op op, cmd_t *left, cmd_t *right);

cmd_t *
create_cmd_with_simple (struct cmd_s *s);

cmd_t *
create_cmd_with_bin_op (struct cmd_b *b);

cmd_t *
create_cmd_with_var_def (struct var_d *v);

#endif