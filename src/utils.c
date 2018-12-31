#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <pwd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utils.h"
#include "types/list.h"

char* strappl(char* str1, ...){
    if (!str1) return NULL;

    va_list ap;
    va_start(ap, str1);

    int* tmp = malloc(sizeof(int));
    *tmp = strlen(str1);
    list_t *head = list_init(tmp, NULL), *tail = head;
    int l = *tmp + 1;
    char* st;
    while ((st = va_arg(ap, char*))){
        tmp = malloc(sizeof(int));
        *tmp = strlen(st);
        l += *tmp;
        tail->next = list_init(tmp, NULL);
        tail = tail->next;
    }
    va_end(ap);

    char* buff = malloc(sizeof(char) * l);
    va_start(ap, str1);
    l = 0;
    st = str1;
    do {
        strcpy(buff + l, st);
        l += *(int*)head->val;
        tail = head;
        head = head->next;
        free(tail->val);
        free(tail);
    } while ((st = va_arg(ap, char*)));

    va_end(ap);
    return buff;
}

char* strappv(char** str){
    if (!str || !str[0]) return NULL;
    int *tmp = malloc(sizeof(int));
    *tmp = strlen(str[0]);
    int l = *tmp + 1;
    list_t* head = list_init(tmp, NULL), *tail = head;

    for(char** st = str + 1; *st; st++){
        tmp = malloc(sizeof(int));
        *tmp = strlen(*st);
        tail->next = list_init(tmp, NULL);
        tail = tail->next;
        l += *tmp;
    }

    char* buff = malloc(sizeof(char) * l);
    l = 0;
    for (char** st = str; *st; st++){
        strcpy(buff + l, *st);
        l += *(int*)head->val;
        tail = head;
        head = head->next;
        free(tail->val);
        free(tail);
    }
    return buff;
}

int log_10 (int n) {
    int l = 0;
    while (n > 0) {
        n /= 10;
        l++;
    }

    return l;
}

short is_positive_number (char *c) {
    while (isdigit(*c)) c++;
    return *c == 0;
}

short is_number (char *c) {
    if (!isdigit(*c) && *c != '-') return 0;
    c++;
    while (isdigit(*c)) c++;
    return *c == 0;
}

// djb2 function from http://www.cse.yorku.ca/~oz/hash.html
unsigned int hash(char *s) {
    unsigned int hash = 5381;
    int c;
    while ((c = *s++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

char* uchar_to_string(unsigned char c){
    char* buff = malloc(sizeof(char) * 4);
    sprintf(buff, "%d", c);
    return buff;
}

int max (int a, int b){
    return a > b ? a : b;
}

char* strrev(char* str){
    if (!str) return NULL;

    int len = strlen(str);
    char* s = malloc(sizeof(char) * (len + 1));
    for (int i = 1; i <= len; i++)
        s[i - 1] = str[len - i];
    s[len] = '\0';

    return s;
}

static char* min(char* a, char* b){
    return a < b ? a : b;
}

char* find_last_str(char* str, char** patterns){
    int len = strlen(str);
    char *rev = strrev(str), *minimum = rev + len, *pattern, *tmp;

    while (*patterns){
        pattern = strrev(*patterns++);
        tmp = strstr(rev, pattern);
        if (tmp)
            minimum = min(minimum, tmp);
        free(pattern);
    }

    if (minimum == rev + len)
        tmp = NULL;
    else
        tmp = str + len - (minimum - rev);

    free(rev);
    return tmp;
}

char *get_dir_from_path(char *path) {
    char *dir;
    for (char *tok = strtok(path, "/"); tok; tok = strtok(0, "/"))
        dir = tok;
    return dir;
}

char *replace_macros(char *str) {
    char c, tmp[256], *tmpp;
    char *buf = calloc(2048, sizeof(char));
    time_t t;
    struct tm tm;
    struct passwd ps;

    while ((c = *str++)) {
        if (c == '\\') {
            switch (*str++) {
            case 'u':
                ps = *getpwuid(geteuid());
                buf = strncat(buf, strdup(ps.pw_name), 1024);
                break;
            case 'h':
                gethostname(tmp, 256);
                buf = strncat(buf, tmp, 1024);
                break;
            case 'w':
                getcwd(tmp, 256);
                buf = strncat(buf, tmp, 1024);
                break;
            case 'W':
                getcwd(tmp, 256);
                tmpp = get_dir_from_path(tmp);
                buf = strncat(buf, tmpp, 1024);
                break;
            case 't':
                t = time(0);
                tm = *localtime(&t);
                sprintf(tmp, "%d:%d:%d", tm.tm_hour, tm.tm_min, tm.tm_sec);
                buf = strncat(buf, tmp, 1024);
                break;
            default: buf[strlen(buf)] = c;
            }
        }
        else buf[strlen(buf)] = c;
    }

    return buf;
}

short is_valid_file_path(char* st){
    struct stat* s = malloc(sizeof(struct stat));
    short ret = stat(st, s) == 0 && (s->st_mode & S_IFMT) == S_IFREG;

    free(s);
    return ret;
}

short is_valid_dir_path(char* st){
    struct stat* s = malloc(sizeof(struct stat));
    short ret = stat(st, s) == 0 && (s->st_mode & S_IFMT) == S_IFDIR;

    free(s);
    return ret;
}
