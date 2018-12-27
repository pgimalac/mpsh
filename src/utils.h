#ifndef UTILS
#define UTILS

#include <stdarg.h>

char *strappl(char*, ...);
char *strappv(char**);
int log_10 (int);
short is_number (char*);
unsigned int hash(char*);
char *uchar_to_string(unsigned char);

#endif
