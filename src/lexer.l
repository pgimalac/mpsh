ws      [ \t]+

special [._\-/$]
alpha   [A-Za-z]
dig     [0-9]
ident   ({alpha}|{dig}|{special})+

%{
#include <stdlib.h>
#include <string.h>

#include "parsing.h"
#include "parser.h"

    extern void yyerror(char *s);
%}

%%
{ws} // skip

{ident}({ws}{ident})*{ws}* {
    int argc, size;
    char *buf;
    char **argv;

    size = 4;
    argc = 0;
    argv = malloc(sizeof(char) * size);

    for (buf = strtok(yytext, " "); buf; buf = strtok(0, " ")) {
	if (argc == size) {
	    size *= 2;
	    argv = realloc(argv, size);
	}

	argv[argc++] = buf;
    }

    argv = realloc(argv, argc);
    yylval.frag = create_cmd_f(argc, argv);
    
    return TOK_FRAG;
}

">" {
    yylval.op = REDIR_WRITE;
    return TOK_REDIR;
}

">>" {
    yylval.op = REDIR_APP;
    return TOK_REDIR;
}

"<>" {
    yylval.op = REDIR_READWRITE;
    return TOK_REDIR;
}

"<" {
    yylval.op = REDIR_READ;
    return TOK_REDIR;
}

"<<" {
    yylval.op = REDIR_HEREDOC;
    return TOK_REDIR;
}

"|" {
    yylval.op = PIPE;
    return TOK_REDIR;
}

"&&" {
    yylval.op = AND;
    return TOK_BINOP;
}

"||" {
    yylval.op = OR;
    return TOK_BINOP;
}

(";"|"\n") {
    return TOK_SEMICOLON;
}

"=" {
    return TOK_EQ;
}

. { 
    yyerror("unexpected character\n");
    return TOK_ERROR;
}
