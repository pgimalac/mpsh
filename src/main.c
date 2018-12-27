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

extern char** environ;

hashmap_t *vars;
hashmap_t *aliases;
hashmap_t *compl;

void init_mpsh() {
    init_completion();
    rl_readline_name = "mpsh";

    rl_attempted_completion_function = fileman_completion;

    vars = hashmap_init();
    aliases = hashmap_init();
    compl = hashmap_init();
    init_env_variables();

    read_history(0);
}

void exit_mpsh(int ret){
    for (char** st = environ; *st; st++)
        free(*st);
    hashmap_destroy(aliases, 1);
    hashmap_destroy(vars, 1);
    hashmap_destroy(compl, 1);

    exit(ret);
}

int main (void) {
    char *s;
    init_mpsh();

    // debug
        char* tmp;
        if ((tmp = get_var("CHEMIN")))
            free(tmp);
        else
            add_var(strdup("CHEMIN"), strdup("/usr/local/bin:/usr/bin:/bin"), 1);

        if ((tmp = get_var("INVITE")))
            free(tmp);
        else
            add_var(strdup("INVITE"), strdup("mpsh> "), 0);
    // end

    char* invite;
    while((s = readline ((invite = get_var("INVITE"))))) {
        command_line_handler(s);
        add_history(s);
        write_history(0);
        free(s);
        free(invite);
    }

    exit_mpsh(0);
}
