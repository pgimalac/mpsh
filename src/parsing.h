/**
 * File managing the parsing of a command line
 */

#ifndef H_PARSER
#define H_PARSER

struct var_d {
    char *name;
    char *value;
};

struct cmd_f {
    int argc;
    char **argv;
};

typedef enum {
      REDIR_WRITE,		/* > */
      REDIR_APP,		/* >> */
      REDIR_ERRWRITE,		/* &>, >& */
      REDIR_READWRITE,		/* <> */
      REDIR_READ,		/* < */
      REDIR_HEREDOC,		/* << */
      REDIR_MERGEIN,		/* <&n */
      REDIR_MERGEOUT,		/* >&n */
      REDIR_CLOSE,		/* >&-, <&- */
} redir_t;

struct redir {
    int fd1, fd2;
    redir_t type;
};

typedef enum
    {
     REDIR,
     PIPE,                     /* | */
     AND,                      /* && */
     OR,                       /* || */
     SEMI                      /* ; or \n */
    } bin_op;

struct cmd_b {
    struct redir *redir;
    struct cmd *left, *right;

    bin_op type;
};

typedef enum
    {
     BIN,
     FRA,
     VAR
    } cmd_type;

typedef struct cmd {
  union {
  	struct var_d *cmd_v;
  	struct cmd_f *cmd_f;
  	struct cmd_b *cmd_b;
  };

  cmd_type type;
} cmd_t;

struct var_d *
create_var_d (char *name, char *value);

struct cmd_f *
create_cmd_f (int argc, char **argv);

struct redir *
create_redir (redir_t type, int fd1, int fd2);

struct cmd_b *
create_cmd_b (bin_op op, cmd_t *left, cmd_t *right);

struct cmd_b *
create_cmd_redir (struct redir *red, cmd_t *left, cmd_t *right);

cmd_t *
create_cmd_with_frag (struct cmd_f *f);

cmd_t *
create_cmd_with_bin_op (struct cmd_b *b);

cmd_t *
create_cmd_with_var_def (struct var_d *v);

#endif
