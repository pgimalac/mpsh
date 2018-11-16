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
    
    struct partial {
	enum bin_op op;
	cmd_t *cmd;
    };
    
    struct partial *create_partial (enum bin_op op, cmd_t *cmd) {
	struct partial *p = malloc(sizeof(struct partial));
	if (p) {
	    p->op = op;
	    p->cmd = cmd;
	}
	
	return p;
    }
%}

%define api.token.prefix {TOK_}
%union {
    cmd_t *cmd;
    struct partial *p;
    struct cmd_f *frag;
    enum bin_op op;
}

%token <frag> FRAG "frag"
%token <op> BINOP "op"
%token <op> REDIR "red"
%token SEMICOLON
%token EQ
%token ERROR

%type <cmd> input
%type <p> cmd
%type <p> cmd2

%left BINOP
%left REDIR

%start input

%%

input:
%empty { $$ = NULL; }
| FRAG cmd {
    if ($2) {
	$$ = create_cmd_with_bin_op
	    (create_cmd_b(($2)->op, create_cmd_with_frag($1), ($2)->cmd));
    } else {
	$$ = create_cmd_with_frag($1);
    }
 }
| ERROR { YYERROR; }
;

cmd:
SEMICOLON input { $$ = create_partial(SEMI, $2); }
| cmd2 { $$ = $1; }
;

cmd2:
%empty { $$ = NULL; }
| BINOP input { $$ = create_partial($1, $2); }
| REDIR input { $$ = create_partial($1, $2); }
| error { YYERROR; }
;

%%
void yyerror(char *s) {
    fprintf(stderr, s);
}
