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
#include "lp/parser.h"
#include "command.h"
#include "env.h"

hashmap_t *vars;
hashmap_t *aliases;

int init_readline() {
    rl_readline_name = "mpsh";

    rl_attempted_completion_function = fileman_completion;
    return 0;
}

int main (void) {
    char *s;

    init_completion();
    init_readline();

    vars = hashmap_init();
    aliases = hashmap_init();
//    hashmap_add(aliases, strdup("a"), strdup("ls"), 1);

    init_env_variables(vars);
    // hashmap_print (vars);

    read_history(0);

    if (!hashmap_get(vars, "CHEMIN"))                                                    // debug
        hashmap_add(vars, strdup("CHEMIN"), strdup("/usr/local/bin:/usr/bin:/bin"), 1);  // debug
    if (!hashmap_get(vars, "INVITE"))                              // debug
        hashmap_add(vars, strdup("INVITE"), strdup("mpsh> "), 1);  // debug

    while((s = readline (hashmap_get(vars, "INVITE")))) {
        command_line_handler(s);
        add_history(s);
        write_history(0);
        free(s);
    }
    return 0;
}
