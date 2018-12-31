#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "completion.h"
#include "types/array.h"
#include "types/hashmap.h"
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

static int create_mpshrc (char* path) {
    int fd = open (path, O_WRONLY | O_CREAT);
    if (fd == -1){
        perror("mpshrc creation");
        return -1;
    }

    const char* default_mpshrc = "export CHEMIN=$PATH\nINVITE=\"[\\u@\\h : \\w]$ \"";

    if (write(fd, default_mpshrc, strlen(default_mpshrc)) == -1){
        perror("mpshrc writing");
        close(fd);
        return -1;
    }

    close(fd);

    return open(path, O_RDONLY);
}

static void handle_mpshrc(){
    char* home = get_var("HOME");
    if (!home) return;

    char* path = strappl(home, "/.mpshrc", NULL);

    int f = open(path, O_RDONLY);
    if (f == -1 && (f = create_mpshrc(path)) == -1)
        perror("mpshrc");
    else
        exec_script(f);

    close(f);
    free(home);
    free(path);
}

static void init_mpsh() {
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
    char *s, *invite, fc;

    if (signal(SIGCHLD, sigchild_handler) == SIG_ERR) {
        perror("mpsh: settings sig child handler");
        return 1;
    }

    init_mpsh();
    while((s = readline ((invite = get_var("INVITE"))))) {
        // first char that is neither a space nor a tab
        fc = *(s + strspn(s, " \t"));
        if (fc != '#' && fc != '\0' && fc != '\n'){
            command_line_handler(s);
            add_history(s);
            write_history(0);
        }
        free(s);
        free(invite);
    }

    exit_mpsh(0);
}
