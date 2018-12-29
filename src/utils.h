#ifndef UTILS
#define UTILS

#include <stdarg.h>

/**
 * Return the concatenation of given strings
 * the list of arguments must be NULL-teminated
 */
char *strappl(char*, ...);

/**
 * Return the concatenation of given strings
 * the list of arguments must be NULL-teminated
 */
char *strappv(char**);

/**
 * Return the number of characters needed to print
 * the given number in base 10
 */
int log_10 (int);

/**
 * Test if the given string is an integer
 */
short is_number (char*);

/**
 * Hash function on NULL-terminated strings
 */
unsigned int hash(char*);

/**
 * Convert an unsigned char into a string
 */
char *uchar_to_string(unsigned char);

/**
 * Replace macros in the given string
 * - \u print the username
 * - \h print the hostname
 * - \w print the full path to the current directory
 * - \W print the current directory
 * - \t print the current time H:m:s
 */
char *replace_macros(char *str);

#endif
