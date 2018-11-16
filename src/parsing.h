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

enum bin_op {
      REDIR_WRITE,		/* > */
      REDIR_APP,		/* >> */
      REDIR_ERRWRITE,		/* &>, >& */
      REDIR_READWRITE,		/* <> */
      REDIR_READ,		/* < */
      REDIR_HEREDOC,		/* << */
      REDIR_MERGEIN,		/* <&n */
      REDIR_MERGEOUT,		/* >&n */
      REDIR_CLOSE,		/* >&-, <&- */
      PIPE,                     /* | */
      AND,                      /* && */
      OR,                       /* || */
      SEMI                      /* ; */
};

struct cmd_b {
    enum bin_op op;
    struct cmd *left, *right;
};

enum cmd_type
    {
     BIN,
     FRA,
     VAR
    };

typedef struct cmd {
    union {
	struct var_d *cmd_v;
	struct cmd_f *cmd_f;
	struct cmd_b *cmd_b;
    };

    enum cmd_type type;
} cmd_t;

struct var_d *
create_var_d (char *name, char *value);

struct cmd_f *
create_cmd_f (int argc, char **argv);

struct cmd_b *
create_cmd_b (enum bin_op op, cmd_t *left, cmd_t *right);

cmd_t *
create_cmd_with_frag (struct cmd_f *f);

cmd_t *
create_cmd_with_bin_op (struct cmd_b *b);

cmd_t *
create_cmd_with_var_def (struct var_d *v);

#endif
