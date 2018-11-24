ws      [ \t]+

special [.~_\-/$]
alpha   [A-Za-z]
dig     [0-9]
escape  \\.
string  (\"({escape}|[^\"\\])*\")|(\'[^\']*\')
var_id  {alpha}({alpha}|{dig})*
ident   ({alpha}|{dig}|{special}|{escape})+

%{
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "parsing.h"
#include "parser.h"

short simplify(char* position){
    char * cursor = position;
    for (cursor = position; *position; position ++)
        if (*position == '\\'){
            if (!*++position)
                return 1;
            *cursor++ = *position;
        } else if (*position == '\'') {
            for (position++; *position && *position != '\''; )
                *cursor++ = *position++;
            if (!*position)
                return 2;
        } else if (*position == '\"') {
            for (position++; *position && *position != '\"'; ){
                if (*position == '\\' && !*++position)
                    return 4;
                *cursor++ = *position++;
            }
            if (!*position)
                return 3;
        } else
            *cursor++ = *position;
    *cursor = '\0';
    return 0;
}

    extern void yyerror(char *s);
%}

%%
{ws} // skip

{dig}">&"{dig} {
    int fd1, fd2;

    fd1 = yytext[0] - '0';
    fd2 = yytext[3] - '0';
    yylval.red = create_redir(REDIR_MERGEOUT, fd1, fd2);
    return TOK_REDIR;
}

">&"{dig} {
    int fd;

    fd = yytext[2] - '0';
    yylval.red = create_redir(REDIR_MERGEOUT, 1, fd);
    return TOK_REDIR;
}

{dig}">" {
    int fd = yytext[0] - '0';
    yylval.red = create_redir(REDIR_WRITE, fd, 0);
    return TOK_REDIR;
}

">" {
    yylval.red = create_redir(REDIR_WRITE, 1, 0);
    return TOK_REDIR;
}

">>" {
    yylval.red = create_redir(REDIR_APP, 1, 0);
    return TOK_REDIR;
}

"<>" {
    yylval.red = create_redir(REDIR_READWRITE, 1, 0);
    return TOK_REDIR;
}

"<" {
    yylval.red = create_redir(REDIR_READ, 1, 0);
    return TOK_REDIR;
}

{dig}"<&"{dig} {
    int fd1, fd2;

    fd1 = yytext[0] - '0';
    fd2 = yytext[3] - '0';
    yylval.red = create_redir(REDIR_MERGEIN, fd1, fd2);
    return TOK_REDIR;
}

"<&"{dig} {
    int fd;

    fd = yytext[2] - '0';
    yylval.red = create_redir(REDIR_MERGEIN, 1, fd);
    return TOK_REDIR;
}

"<<" {
    yylval.red = create_redir(REDIR_HEREDOC, 1, 0);
    return TOK_REDIR;
}

"|" {
    yylval.op = PIPE;
    return TOK_PIPE;
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

{var_id}"="({ident}|{string}) {
    char *name = strtok(yytext, "=");
    char *value = strtok(0, "=");
    simplify(name);
    simplify(value);
    yylval.var = create_var_d(name, value);
    return TOK_VAR;
}

{ident}({ws}({ident}|{string}))*{ws} {
    int argc;
    char *buf;
    char **argv;
    array_t *args = array_init();

    for (buf = strtok(yytext, " "); buf; buf = strtok(0, " "))
        array_add(args, buf);

    argc = args->size;
    argv = array_to_tab(args);
    yylval.frag = create_cmd_f(argc, argv);
    return TOK_FRAG;
}

. {
    fprintf(stderr, "mpsh: unexpected character %s\n", yytext);
    return TOK_ERROR;
}
