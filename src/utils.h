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

/*
 * Returns if the given string is a number
 */
short is_number (char*);

/**
 * Returns a hash for the given string
 */
unsigned int hash(char*);

/**
 * Converts an unsigned char into a string
 */
char* uchar_to_string(unsigned char);

/**
 * Returns a pointer to the end of the last appearance of any of the patterns in the string.
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

#endif
