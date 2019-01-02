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

#define DEFAULT_INVITE "[\\u@\\h : \\W]$ "
#define DEFAULT_CHEMIN "/bin/usr/bin:/usr/local/bin"
#define DEFAULT_MPSHRC "export CHEMIN=$PATH\nINVITE=\"[\\u@\\h : \\W]$ \""

extern char** environ;

hashmap_t *vars;
hashmap_t *aliases;
hashmap_t *compl;

static int create_mpshrc (char* path) {
    printf("~/.mpshrc not found.\nCreation of a default .mpshrc\nThe default content is\n\n%s\n\n", DEFAULT_MPSHRC);
    int fd = open (path, O_WRONLY | O_CREAT);
    if (fd == -1){
        perror("mpsh: mpshrc creation");
        return -1;
    }

    if (write(fd, DEFAULT_MPSHRC, strlen(DEFAULT_MPSHRC)) == -1){
        perror("mpsh: mpshrc writing");
        close(fd);
        return -1;
    }
    printf("~/.mpshrc created.\n");
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
    else {
        printf("Execution of the ~/.mpshrc script.\n");
        if (exec_script(f) == 0)
            printf("~/.mpshrc successfully executed.\n");
        else
            printf("Problem in the execution of ~/.mpshrc.\n");
    }

    close(f);
    free(home);
    free(path);
}

static void init_mpsh() {
//    init_completion();
    rl_readline_name = "mpsh";

    rl_attempted_completion_function = fileman_completion;
    rl_ignore_some_completions_function = find_files_with_ext;

    vars = hashmap_init();
    aliases = hashmap_init();
    compl = hashmap_init();
    init_env_variables();

    read_history(0);

    handle_mpshrc();

    char* tmp;
    // Default value of INVITE
    tmp = get_var("INVITE");
    if (tmp)
        free(tmp);
    else {
        printf("$INVITE set to its default value \"%s\"\n", DEFAULT_INVITE);
        add_var(strdup("INVITE"), strdup(DEFAULT_INVITE), 0);
    }

    // Default value of CHEMIN
    tmp = get_var("CHEMIN");
    if (tmp)
        free(tmp);
    else {
        printf("$CHEMIN set to its default value \"%s\"\n", DEFAULT_CHEMIN);
        add_var(strdup("CHEMIN"), strdup(DEFAULT_CHEMIN), 1);
    }
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

    return 0;
}
