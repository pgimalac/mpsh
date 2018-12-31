#ifndef UTILS
#define UTILS

#include <stdarg.h>

/**
 * Appends all the string given in parameter.
 * Last argument must be NULL.
 * The returned string is obtained with malloc.
 */
char* strappl(char*, ...);

/**
 * Appends all the string of the NULL-terminated array of strings.
 * The returned string is obtained with malloc.
 */
char* strappv(char**);

/**
 * Returns the log 10 of the given int
 */
int log_10 (int);

/**
 * Returns if the given string is a number
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
 * Test if the given string is a positive integer
 */
short is_positive_number (char*);

/**
 * Test if the given string is an integer
 */
short is_number (char*);

/**
 * Hash function on NULL-terminated strings
 */
unsigned int hash(char*);

/**
 * Replaces the macros in a string
 * The returned string is obtained with malloc.
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

/**
 * Returns a pointer to the end of the last appearance
 * of any of the patterns in the string.
 * The pattern list must be NULL terminated.
 */
char* find_last_str(char*, char**);

/**
 * Returns the maximum of the two ints
 */
int max(int, int);

/**
 * Returns the given string reversed
 * Obtained with malloc.
 */
char* strrev(char*);

/**
 * Returns if the given string is a path to an existing regular file
 */
short is_valid_file_path(char*);

/**
 * Returns if the given string is a path to an existing directory
 */
short is_valid_dir_path(char*);

#endif
