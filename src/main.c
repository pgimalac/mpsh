#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "completion.h"
#include "array.h"
#include "hashmap.h"
#include "parsing.h"
#include "parser.h"
#include "command.h"
#include "env.h"

hashmap_t *vars;

int
init_readline() {
    rl_readline_name = "mpsh";

    rl_attempted_completion_function = fileman_completion;
    return 0;
}

int
main (void) {
    char *s;

    init_completion();
    init_readline();
    vars = hashmap_init();
    init_env_variables(vars);
    // hashmap_print (vars);

    while((s = readline ("mpsh> "))) {
        command_line_handler(s);
        add_history(s);
        free(s);
    }
    return 0;
}
