#include <signal.h>
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
#include "utils.h"

extern char** environ;
extern char** matches;
extern char* command;

hashmap_t *vars;
hashmap_t *aliases;
hashmap_t *compl;

void handle_mpshrc(){
    // will be done soon
}

void init_mpsh() {
    init_completion();
    rl_readline_name = "mpsh";

    rl_attempted_completion_function = fileman_completion;
    rl_ignore_some_completions_function = find_files_with_ext;

    vars = hashmap_init();
    aliases = hashmap_init();
    compl = hashmap_init();
    init_env_variables();

    read_history(0);

    handle_mpshrc();
}

void exit_mpsh(int ret){
    for (char** st = environ; *st; st++)
        free(*st);
    if (matches){
        for (char** st = matches; *st; st++)
            free(*st);
        free(matches);
    }

    free(command);

    hashmap_destroy(aliases, 1);
    hashmap_destroy(vars, 1);
    hashmap_destroy(compl, 1);

    exit(ret);
}

int main (void) {
    char *s, *invite;

    if (signal(SIGCHLD, sigchild_handler) == SIG_ERR) {
        perror("mpsh: settings sig child handler");
        return 1;
    }

    init_mpsh();

    while((s = readline ((invite = get_var("INVITE"))))) {
        if (s[0]){
            command_line_handler(s);
            add_history(s);
            write_history(0);
        }
        free(s);
        free(invite);
    }

    exit_mpsh(0);
}
