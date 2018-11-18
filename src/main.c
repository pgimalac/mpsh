#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "arrayList.h"

char *dupstr(char *s){
  char *r = malloc(strlen(s) + 1);
  strcpy (r, s);
  return r;
}

#define MAX_COMPLETION 10000

char *completions[MAX_COMPLETION];
int nb_completions;

void init_completion () {
  DIR *dir;
  struct dirent *entry;
  char *var_path = getenv("PATH");
  char *path, *buf, *pathname;
  char *saveptr1, *saveptr2;
  struct stat stat_buf, stat_buf2;

  nb_completions = 0;
  for (int i = 0; i < MAX_COMPLETION; i++)
    completions[i] = 0;

  for (path = strtok_r(var_path, ":", &saveptr1);; var_path = 0,
     path = strtok_r(0, ":", &saveptr1)) {

    if (!path) break;

    if (stat(path, &stat_buf) == 0) {
      if (S_ISDIR(stat_buf.st_mode)) {

        if ((dir = opendir(path))) {
          while (nb_completions < MAX_COMPLETION &&
             (entry = readdir(dir)))

            if (entry->d_type == DT_REG) { // TODO: check if exec
              pathname = malloc(sizeof(char) * 512);
              strncpy(pathname, path, 256);
              strcat(pathname, "/");
              strncat(pathname, entry->d_name, 256);

              if(stat(pathname, &stat_buf2) == 0 &&
                 stat_buf2.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {

                buf = malloc(sizeof(char) * 256);
                strncpy(buf, entry->d_name, 255);
                completions[nb_completions++] = buf;
              }

              free(pathname);
            }
        } else fprintf(stderr, "Can't open %s", path);

        closedir(dir);
      } else if (nb_completions < MAX_COMPLETION &&
           S_ISREG(stat_buf.st_mode) &&
           stat_buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        char *name, *tmp;
        for (tmp = strtok_r(path, "/", &saveptr2); tmp; path = 0,
           tmp = strtok_r(0, "/", &saveptr2))
          name = tmp;

        completions[nb_completions++] = name;
      }
    } else fprintf(stderr, "Can't open %s", path);
  }
}

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
  end = end; // remove Werror=unused-parameter

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
  arrayList *a = malloc(sizeof(arrayList));
  arrayList_init(a);

  printf("1 : %d\n", arrayList_add(a, "a")); // 1
  printf("2 : %d\n", arrayList_add(a, "b")); // 2
  printf("3 : %d\n", arrayList_add(a, "c")); // 3
  printf("4 : %d\n", arrayList_add(a, "d")); // 4
  printf("5 : %d\n", arrayList_add(a, "e")); // 5
  printf("6 : %d\n", arrayList_add(a, "f")); // 6
  printf("7 : %d\n", arrayList_add(a, "g")); // 7
  printf("8 : %d\n", arrayList_add(a, "h")); // 8
  printf("9 : %d\n", arrayList_add(a, "i")); // 9
  printf("10 : %d\n", arrayList_add(a, "j")); // 10
  printf("11 : %d\n", arrayList_add(a, "k")); // 11
  printf("12 : %d\n", arrayList_add(a, "l")); // 12
  printf("13 : %d\n", arrayList_add(a, "m")); // 13
  printf("14 : %d\n", arrayList_add(a, "n")); // 14
  printf("15 : %d\n", arrayList_add(a, "o")); // 15
  printf("16 : %d\n", arrayList_add(a, "p")); // 16
  printf("17 : %d\n", arrayList_add(a, "q")); // 17
  printf("18 : %d\n", arrayList_add(a, "r")); // 18
  printf("19 : %d\n", arrayList_add(a, "s")); // 19
  printf("20 : %d\n", arrayList_add(a, "t")); // 20
  printf("21 : %d\n", arrayList_add(a, "u")); // 21
  printf("22 : %d\n", arrayList_add(a, "v")); // 22
  printf("23 : %d\n", arrayList_add(a, "w")); // 23
  printf("24 : %d\n", arrayList_add(a, "x")); // 24
  printf("25 : %d\n", arrayList_add(a, "y")); // 25
  printf("26 : %d\n", arrayList_add(a, "z")); // 26

  printf("%d\n", arrayList_removeIndex(a, 25)); // z
  printf("%d\n", arrayList_removeIndex(a, 0));  // a
  printf("%d\n", arrayList_removeIndex(a, 14)); // n

  printf("%d\n", arrayList_remove(a, "b"));
  printf("%d\n", arrayList_remove(a, "y"));
  printf("%d\n", arrayList_remove(a, "c"));

  printf("%d\n", arrayList_set(a, 0, "hello "));
  printf("%d\n", arrayList_set(a, 5, "world "));
  printf("%d\n", arrayList_set(a, 17, "!"));

  printf("%d\n", arrayList_contains(a, "n"));
  for (int i = 0; i < a->size; i++)
    printf("%s\n", a->tab[i]);
  printf("\n");

/*  char *s;

  init_completion();
  init_readline();

  while(1) {
    s = readline ("mpsh> ");
    command_line_handler(s);
    free(s);
  }
*/
  return 0;
}
