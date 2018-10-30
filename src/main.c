#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

char *dupstr(char *s){
    char *r = malloc(strlen(s)+1);
    strcpy (r, s);
    return r;
}

char *completions[] = { "echo", "cat" };
int nb_completions = 2;

char *command_generator (const char *com, int num){
    static int indice, len;
    char *completion;

    if (num == 0){
	indice = 0;
	len = strlen(com);
    }

    while (indice < nb_completions) {
	completion = completions[indice++];

	if (strncmp (completion, com, len) == 0)
	    return dupstr(completion);
    }
  
    return NULL;
}

char ** fileman_completion (const char *com, int start, int end) {
    char **matches;
    matches = (char **)NULL;
    end++; // remove Werror=unused-parameter

    if (start == 0)
	matches = rl_completion_matches (com, command_generator);

    return matches;
}

int init_readline() {
    rl_readline_name = "mpsh";

    rl_attempted_completion_function = fileman_completion;
    return 0;
}

void command_line_handler () {
    // TODO: fill with stuffs
}


int main (void) {
    char *s;

    init_readline();

    while(1) {
	s = readline ("mpsh> ");
	command_line_handler(s);
	free(s);
    }
    
    return 0;
}
