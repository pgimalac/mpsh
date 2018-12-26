#ifndef ENV
#define ENV

void init_env_variables ();
char* secure_get_var(char*);
char* get_var (char*);
void add_var (char*, char*, short);

#endif
