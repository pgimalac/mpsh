/**
 * Yacc parser for command-line
 */

%{
#include <stdio.h>
#include <stdlib.h>

#include "parsing.h"

    extern int yylex();
    extern void yyerror(char *s);
    int yywrap () { return 1; }
    
    cmd_t* parse_ret;
%}

%define api.token.prefix {TOK_}
%union {
    cmd_t *cmd;
    struct cmd_f *frag;
    struct var_d *var;
    struct redir *red;
    bin_op op;
}

%token <var> VAR
%token <frag> FRAG
%left <op> BINOP
%left <op> PIPE
%left <red> REDIR REDIR_SIMPLE
%token SEMICOLON
%token ERROR

%type <cmd> input
%type <cmd> cmd cmd_simple

%start input

%%

input:
cmd			{ parse_ret = $1; }
;

cmd:
  VAR			{ $$ = create_cmd_with_var_def($1); }
| cmd_simple            { $$ = $1; }
| cmd PIPE cmd		{ $$ = create_cmd_with_bin_op(create_cmd_b($2, $1, $3)); }
| cmd BINOP cmd		{ $$ = create_cmd_with_bin_op(create_cmd_b($2, $1, $3)); }
| cmd SEMICOLON cmd	{ $$ = create_cmd_with_bin_op(create_cmd_b(SEMI, $1, $3)); }
| error			{ return 1; }
;

cmd_simple:
%empty                { $$ = 0; }
| FRAG { $$ = create_cmd_with_frag($1); }
| REDIR_SIMPLE cmd_simple
{ $$ = create_cmd_with_bin_op(create_cmd_redir($1, 0, $2)); }
| cmd_simple REDIR cmd_simple
{ $$ = create_cmd_with_bin_op(create_cmd_redir($2, $1, $3)); }
;

%%
void yyerror(char *s) {
    fprintf(stderr, "mpsh: %s\n", s);
}
