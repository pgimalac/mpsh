%{
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "parsing.h"
#include "parser.h"

    extern void yyerror(char *s);
%}

ws      [ \t]+

special [.~_\-/$]
alpha   [A-Za-z]
dig     [0-9]
escape  \\.
var_id  {alpha}({alpha}|{dig})*
ident   ({alpha}|{dig}|{special})+

%x STRING
%%

<INITIAL>\" { BEGIN(STRING); }
<STRING>\" {
    yylval.value = strndup(yytext, strlen(yytext) - 1);
    BEGIN(INITIAL);
    return TOK_IDENT;
 }
<STRING>[^\"] { yymore(); }
<STRING><<EOF>> {
    yyerror("expected \" before EOF");
    return 1;
 }

{ws} // skip

{dig}">&"{dig} {
    int fd1, fd2;

    fd1 = yytext[0] - '0';
    fd2 = yytext[3] - '0';
    yylval.simple = create_simple_redir(REDIR_MERGEOUT, fd1, fd2);
    return TOK_SIMPLE_REDIR;
}

">&"{dig} {
    int fd;

    fd = yytext[2] - '0';
    yylval.simple = create_simple_redir(REDIR_MERGEOUT, 1, fd);
    return TOK_SIMPLE_REDIR;
}

{dig}">" {
    int fd = yytext[0] - '0';
    yylval.file = create_file_redir(REDIR_WRITE, fd, 0);
    return TOK_REDIR;
}

">" {
    yylval.file = create_file_redir(REDIR_WRITE, 1, 0);
    return TOK_REDIR;
}

">>" {
    yylval.file = create_file_redir(REDIR_APP, 1, 0);
    return TOK_REDIR;
}

"<>" {
    yylval.file = create_file_redir(REDIR_READWRITE, 1, 0);
    return TOK_REDIR;
}

"<" {
    yylval.file = create_file_redir(REDIR_READ, 1, 0);
    return TOK_REDIR;
}

{dig}"<&"{dig} {
    int fd1, fd2;

    fd1 = yytext[0] - '0';
    fd2 = yytext[3] - '0';
    yylval.simple = create_simple_redir(REDIR_MERGEIN, fd1, fd2);
    return TOK_SIMPLE_REDIR;
}

"<&"{dig} {
    int fd;

    fd = yytext[2] - '0';
    yylval.file = create_file_redir(REDIR_MERGEIN, fd, 0);
    return TOK_REDIR;
}

"<<" {
    yylval.file = create_file_redir(REDIR_HEREDOC, 1, 0);
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

{ident} {
    yylval.value = strdup(yytext);
    return TOK_IDENT;
}

. {
    fprintf(stderr, "mpsh: unexpected character %s\n", yytext);
    return TOK_ERROR;
}
