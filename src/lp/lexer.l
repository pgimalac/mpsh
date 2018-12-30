%{
#include <stdlib.h>
#include <string.h>

#include "hashset.h"
#include "hashmap.h"
#include "parsing.h"
#include "parser.h"
#include "env.h"
#include "utils.h"

    extern hashmap_t *vars, *aliases;
    extern void yyerror(char *s);
    int alias_depth = 0, alias_done = 0;
    YY_BUFFER_STATE buffer_stack[100];
    hashset_t *used;
%}

ws      [ \t]+
special [.~_\-/]
alpha   [A-Za-z]
dig     [0-9]
escape  \\.
var_id  ({alpha}|\?)({alpha}|{dig})*
ident   [^<>|&;$\\" "=]+

%x IDENT
%x STRING
%%

<INITIAL,IDENT>"?" {
    yylval.value = strdup(secure_get_var("?"));
    BEGIN INITIAL;
    return TOK_IDENT;
}

<INITIAL,IDENT>"~" {
    yylval.value = strdup(secure_get_var("HOME"));
    BEGIN INITIAL;
    return TOK_IDENT;
}

<INITIAL,IDENT>"$"{var_id} {
    char *value = secure_get_var(yytext + 1);
    value = value ? value : yytext;
    if (YY_START == INITIAL) {
        yylval.value = value;
        BEGIN IDENT;
    } else {
        yylval.value = strappl(yylval.value, value, NULL);
    }
}

<INITIAL>\" { yylval.value = "";BEGIN(STRING); }
<STRING>\" {
    BEGIN(INITIAL);
    return TOK_IDENT;
}
<STRING>[^\"] {
    yylval.value = strappl(yylval.value, yytext, NULL);
}
<STRING><<EOF>> {
    BEGIN INITIAL;
    yyerror("unclosed string");
    return 1;
}

{ws} // skip
"&" return TOK_BG;
(";"|"\n") return TOK_SEMICOLON;
"=" return TOK_EQ;

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
    alias_done = 0;
    yylval.op = PIPE;
    return TOK_PIPE;
}

"&&" {
    alias_done = 0;
    yylval.op = AND;
    return TOK_BINOP;
}

"||" {
    alias_done = 0;
    yylval.op = OR;
    return TOK_BINOP;
}

<INITIAL>{ident} {
    yylval.value = strdup(yytext);
    BEGIN IDENT;
}

<IDENT>{ident} {
    yylval.value = strappl(yylval.value, yytext, NULL);
}

<IDENT>\\. {
    yylval.value = strappl(yylval.value, yytext + 1, NULL);
 }

<IDENT>. {
    yyless(0);
    if (alias_done) {
        BEGIN INITIAL;
        return TOK_IDENT;
    }

    if (!used) used = hashset_init();
    else if (hashset_contains(used, yylval.value)) {
        BEGIN INITIAL;
        return TOK_IDENT;
    }

    char *alias = hashmap_get(aliases, yylval.value);
    if (alias) {
        hashset_add(used, strdup(alias));
        buffer_stack[alias_depth++] = YY_CURRENT_BUFFER;
        yy_switch_to_buffer(yy_scan_string(strdup(alias)));
        BEGIN INITIAL;
    } else {
        alias_done = 1;
        BEGIN INITIAL;
        return TOK_IDENT;
    }
}

<IDENT><<EOF>> {
    if (alias_done) {
        BEGIN INITIAL;
        return TOK_IDENT;
    }

    if (!used) used = hashset_init();
    else if (hashset_contains(used, yylval.value)) {
        BEGIN INITIAL;
        return TOK_IDENT;
    }

    char *alias = hashmap_get(aliases, yylval.value);
    if (alias) {
        hashset_add(used, strdup(alias));
        buffer_stack[alias_depth++] = YY_CURRENT_BUFFER;
        yy_switch_to_buffer(yy_scan_string(strdup(alias)));
        BEGIN INITIAL;
    } else {
        alias_done = 1;
        BEGIN INITIAL;
        return TOK_IDENT;
    }
}


<<EOF>> {
    if (alias_depth > 0) {
        alias_done = 1;
        yy_delete_buffer(YY_CURRENT_BUFFER);
        yy_switch_to_buffer(buffer_stack[--alias_depth]);
    } else {
        alias_done = 0;
        hashset_destroy(used, 1);
        used = 0;
        yyterminate();
    }
}

. {
    fprintf(stderr, "mpsh: unexpected character %s\n", yytext);
    return TOK_ERROR;
}
