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

int init_readline() {
    rl_readline_name = "mpsh";

    rl_attempted_completion_function = fileman_completion;
    return 0;
}

void exit_mpsh(int ret){
    for (char** st = environ; *st; st++)
        free(*st);
    hashmap_destroy(aliases, 1);
    hashmap_destroy(vars, 1);

    exit(ret);
}

int main (void) {
    char *s, *invite, *tmp;

    init_completion();
    init_readline();

    vars = hashmap_init();
    aliases = hashmap_init();
    //    hashmap_add(aliases, strdup("a"), strdup("ls"), 1);

    init_env_variables();
    // hashmap_print (vars);

    read_history(0);
    if ((tmp = get_var("CHEMIN"))) free(tmp);
    else add_var(strdup("CHEMIN"), strdup("/usr/local/bin:/usr/bin:/bin"), 1);

    if ((tmp = get_var("INVITE"))) free(tmp);
    else add_var(strdup("INVITE"), strdup("mpsh> "), 0);

    invite = get_var("INVITE");
    while((s = readline (invite))) {
        command_line_handler(s);
        add_history(s);
        write_history(0);
        free(s);
        free(invite);
        invite = get_var("INVITE");
    }

    exit_mpsh(0);
}
