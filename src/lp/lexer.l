%{
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "parsing.h"
#include "parser.h"

    static char *cat(char *str1, char *str2) {
        char *buf = malloc(strlen(str1) + strlen(str2) + 1);
        if (buf) {
            strcpy(buf, str1);
            strcat(buf, str2);
        }

        return buf;
    }

    extern hashmap_t *vars;
    extern void yyerror(char *s);

    static char *var_name_buf;
%}

ws      [ \t]+
special [.~_\-/]
alpha   [A-Za-z]
dig     [0-9]
escape  \\.
var_id  {alpha}({alpha}|{dig})*
ident   ({alpha}|{dig}|{special})+

%x VAR
%x IDENT
%x STRING
%%

<*>"$"{var_id} {
    char *value = hashmap_get(vars, yytext + 1);
    value = value ? value : yytext;
    if (YY_START == INITIAL) {
        yylval.value = value;
        BEGIN IDENT;
    } else {
        yylval.value = cat(yylval.value, value);
    }
}

<INITIAL>\" { yylval.value = "";BEGIN(STRING); }
<STRING>\" {
    BEGIN(INITIAL);
    return TOK_IDENT;
}
<STRING>[^\"] {
    yylval.value = cat(yylval.value, yytext);
}
<STRING><<EOF>> {
    yyerror("unclosed string");
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

<INITIAL>{ident} {
    yylval.value = strdup(yytext);
    BEGIN IDENT;
}

<IDENT>{ident} {
    yylval.value = cat(yylval.value, yytext);
}

<IDENT>\\. {
    yylval.value = cat(yylval.value, yytext + 1);
 }

<IDENT>. {
    yyless(0);
    BEGIN INITIAL;
    return TOK_IDENT;
}

<IDENT><<EOF>> {
    BEGIN INITIAL;
    return TOK_IDENT;
 }

{var_id}"=" {
    yylval.value = "";
    var_name_buf = strndup(yytext, strlen(yytext) - 1);
    BEGIN VAR;
}

<VAR>[^"\n"$]+ {
    yylval.value = cat(yylval.value, yytext);
}

<VAR><<EOF>> {
    yylval.var = create_var_d(var_name_buf, yylval.value);
    BEGIN INITIAL;
    return TOK_VAR;
 }

. {
    fprintf(stderr, "mpsh: unexpected character %s\n", yytext);
    return TOK_ERROR;
}
