#ifndef ENV
#define ENV

/**
 * Initialise environnement variables.
 * Must be called before beginning main loop.
 */
void init_env_variables();

/**
 * Return a replicate of the variable value.
 */
char *secure_get_var(char *);

/**
 * Return the value of the given variable.
 * Obtained with malloc.
 */
char *get_var(char *);

/**
 * Takes the variable name, his value, and a short.
 * Both the key and the value must be free-able and
 * not touched after given to this function.
 * If the short is 0, then the variable is added as what it was
 * (or local variable if it wasn't set).
 * If the short is 1, the variable is added as an environnement variable
 * (possibly removed from local variables).
 * If the value is NULL, then it is replaced with either
 *  the former value (if the variable already exists) or the empty string
 */
void add_var(char *, char *, short);

#endif
