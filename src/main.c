#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "array.h"
#include "hashmap.h"
#include "parsing.h"
#include "parser.h"
#include "path.h"
#include "cmd.h"

extern int yy_scan_string(const char *);
extern cmd_t *parse_ret;

hashmap_t *vars;
char **completions;
int nb_completions;
char *path;

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
            return strdup(completion);
    }

    return NULL;
}

char ** fileman_completion (const char *com, int start, int end) {
    char **matches;
    matches = (char **)NULL;
    end = end;

    if (start == 0)
        matches = rl_completion_matches (com, command_generator);

    return matches;
}

int init_readline() {
    rl_readline_name = "mpsh";

    rl_attempted_completion_function = fileman_completion;
    return 0;
}

void command_line_handler (char* input) {
    if (!input) return;

    yy_scan_string(input);
    if (yyparse() != 0) return;

    exec_cmd(parse_ret);
}

int main (void) {
    char *s;

    path = strdup(getenv("PATH"));
    array_t *completions_array = get_all_files(path);
    nb_completions = completions_array->size;
    completions = array_to_tab(completions_array);

    init_readline();
    vars = hashmap_init();
    hashmap_add(vars, "v1", "coucou");
    // TODO: add env variables

    while((s = readline ("mpsh> "))) {
        command_line_handler(s);
        add_history(s);
        free(s);
    }
    return 0;
}
