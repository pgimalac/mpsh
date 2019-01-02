/**
 * Yacc parser for command-line
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "parsing.h"
#include "utils.h"

    extern int yylex();
    extern void yyerror(char *s);
    int yywrap () { return 1; }

    cmd_t* parse_ret;
%}

%define api.token.prefix {TOK_}
%union {
    cmd_t *cmd;
    char *value;

    list_t *arg_list;
    list_t *redir_list;

    struct simple_redir *simple;
    struct file_redir *file;
    struct cmd_f *frag;
    struct var_d *var;
    struct redir *red;
    bin_op op;
}

%token <var> VAR
%token <value> IDENT
%left <op> BINOP
%right <op> PIPE
%left <file> REDIR
%left <simple> SIMPLE_REDIR
%token SEMICOLON
%token ERROR
%token EQ BG
%token WS

%type <cmd> input
%type <cmd> cmd cmd_simple
%type <redir_list> redir
%type <arg_list> args

%start input

%%

input:
cmd                 { parse_ret = $1; }
;

cmd:
  IDENT EQ IDENT    { $$ = create_cmd_with_var_def(create_var_d($1, $3)); }
| cmd_simple        { $$ = $1; }
| cmd PIPE cmd      { $$ = create_cmd_with_bin_op(create_cmd_b($2, $1, $3)); }
| cmd BINOP cmd     { $$ = create_cmd_with_bin_op(create_cmd_b($2, $1, $3)); }
| cmd SEMICOLON cmd { $$ = create_cmd_with_bin_op(create_cmd_b(SEMI, $1, $3)); }
;

cmd_simple:
%empty              { $$ = 0; }
| args redir BG {
    char **argv = (char**)list_to_tab($1, sizeof(char *));
    $$ = create_cmd_with_simple(create_cmd_s(argv, $2, 1));
  }
| args redir {
    char **argv = (char**)list_to_tab($1, sizeof(char *));
    $$ = create_cmd_with_simple(create_cmd_s(argv, $2, 0));
  }
;

redir:
%empty { $$ = 0; }
| SIMPLE_REDIR redir {
    list_add(&$2, redir_from_simple($1));
    $$ = $2;
}
| REDIR IDENT redir {
    $1->fname = $2;
    list_add(&$3, redir_from_file($1));
    $$ = $3;
  }
| error { yyerror ("expecting redirection"); return 1;}
;

args:
  IDENT          { $$ = list_init($1, 0); }
| IDENT EQ IDENT {
    char *s = strappl($1, "=", $3, NULL);
    $$ = list_init(s, 0);
  }
| IDENT args {
    list_add(&$2, $1);
    $$ = $2;
  }
| error { yyerror ("expecting argument"); return 1;}
;

%%
void yyerror(char *s) {
    fprintf(stderr, "mpsh: %s\n", s);
}
