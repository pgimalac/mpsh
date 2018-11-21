#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "list.h"
#include "array.h"

char *dupstr(char *s){
  char *r = malloc(strlen(s) + 1);
  strcpy (r, s);
  return r;
}

char **completions;
int nb_completions;

void init_completion () {
  DIR *dir;
  struct dirent *entry;
  char *var_path = getenv("PATH");
  char *path, *buf, *pathname;
  char *saveptr1, *saveptr2;
  struct stat stat_buf, stat_buf2;
  array_t *array = array_init();

  for (path = strtok_r(var_path, ":", &saveptr1); path; var_path = 0,
	   path = strtok_r(0, ":", &saveptr1)) {

      if (stat(path, &stat_buf) == 0) {
	  if (S_ISDIR(stat_buf.st_mode)) {

	      if ((dir = opendir(path))) {
		  while ((entry = readdir(dir)))
		      if (entry->d_type == DT_REG) {
			  pathname = malloc(sizeof(char) * 512);
			  strncpy(pathname, path, 256);
			  strcat(pathname, "/");
			  strncat(pathname, entry->d_name, 256);

			  if(stat(pathname, &stat_buf2) == 0 &&
			     stat_buf2.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {

			      buf = malloc(sizeof(char) * 256);
			      strncpy(buf, entry->d_name, 255);
			      array_add(array, buf);
			  }

			  free(pathname);
		      }
	      } else fprintf(stderr, "Can't open %s", path);

	      closedir(dir);
	  } else if (S_ISREG(stat_buf.st_mode) &&
		     stat_buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
	      char *name, *tmp;
	      for (tmp = strtok_r(path, "/", &saveptr2); tmp; path = 0,
		       tmp = strtok_r(0, "/", &saveptr2))
		  name = tmp;
	      array_add(array, name);
	  }
      } else fprintf(stderr, "Can't open %s", path);
  }

  nb_completions = array->size;
  completions = array_to_tab(array);
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
  /* list_t *l = list_init("a", NULL); */

  /* printf("1 %d\n", list_add(&l, "b")); // 2 */
  /* printf("2 %d\n", list_add(&l, "c")); // 3 */
  /* printf("3 %d\n", list_add(&l, "d")); // 4 */
  /* printf("4 %d\n", list_add(&l, "e")); // 5 */
  /* printf("5 %d\n", list_add(&l, "f")); // 6 */
  /* printf("6 %d\n", list_add(&l, "g")); // 7 */
  /* printf("7 %d\n", list_add(&l, "h")); // 8 */
  /* printf("8 %d\n", list_add(&l, "i")); // 9 */
  /* printf("9 %d\n", list_add(&l, "j")); // 10 */
  /* printf("10 %d\n", list_add(&l, "k")); // 11 */
  /* printf("11 %d\n", list_add(&l, "l")); // 12 */
  /* printf("12 %d\n", list_add(&l, "m")); // 13 */
  /* printf("13 %d\n", list_add(&l, "n")); // 14 */
  /* printf("14 %d\n", list_add(&l, "o")); // 15 */
  /* printf("15 %d\n", list_add(&l, "p")); // 16 */
  /* printf("16 %d\n", list_add(&l, "q")); // 17 */
  /* printf("17 %d\n", list_add(&l, "r")); // 18 */
  /* printf("18 %d\n", list_add(&l, "s")); // 19 */
  /* printf("19 %d\n", list_add(&l, "t")); // 20 */
  /* printf("20 %d\n", list_add(&l, "u")); // 21 */
  /* printf("21 %d\n", list_add(&l, "v")); // 22 */
  /* printf("22 %d\n", list_add(&l, "w")); // 23 */
  /* printf("23 %d\n", list_add(&l, "x")); // 24 */
  /* printf("24 %d\n", list_add(&l, "y")); // 25 */
  /* printf("25 %d\n", list_add(&l, "z")); // 26 */

  /* printf("%d\n", list_remove(&l, 0)); // z */
  /* printf("%d\n", list_remove(&l, 24));  // a */
  /* printf("%d\n", list_remove(&l, 15)); // n */

  /* for (list_t* li = l; li != NULL; li = li->next) */
  /*   printf("%s\n", (char*)li->val); */

  char *s;

  init_completion();
  init_readline();

  while(1) {
    s = readline ("mpsh> ");
    command_line_handler(s);
    free(s);
  }
  return 0;
}
