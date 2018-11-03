#include <stdlib.h>
#include <string.h>

#include "parser.h"

#define MAX_ARGV 100

command *create_command (int argc, char *argv[MAX_ARGV]) {
    char **buf;
    command *cmd = malloc(sizeof(command));

    if (cmd) {
	cmd->argc = argc;
	buf = malloc(sizeof(char*) * (argc + 1));
	for (int i = 0; i < argc; i++) {
	    buf[i] = malloc(sizeof(char) * strlen(argv[i]));
	    strcpy(buf[i], argv[i]);
	}
	buf[argc] = 0;
	cmd->argv = buf;
    }

    return cmd;
}

command_node *create_node
(enum b_type t, command *cmd, command_node *prev, command_node *next) {
    command_node *ret = malloc(sizeof(command_node));
    if (ret) {
	ret->type = t;
	ret->cmd = cmd;
	ret->prev = prev;
	if (prev) prev->next = ret;
	ret->next = next;
	if (next) next->prev = ret;
    }

    return ret;
}

command_queue *create_queue () {
    command_queue *queue = malloc(sizeof(command_queue));
    if (queue) {
	queue->first = 0;
	queue->last = 0;
    }

    return queue;
}

void add(command_queue *queue, enum b_type t, command *cmd) {
    command_node *q, *n;

    q = queue->last;

    if (!q) {
	q = create_node(t, cmd, 0, 0);
	queue->first = q;
	queue->last = q;
    } else {
	n = create_node(t, cmd, q, 0);
	queue->last = n;
    }
}

command_node* get (command_queue *queue) {
    command_node *ret = queue->first;

    if (ret) {
	queue->first = ret->next;
	if (!ret->next) queue->last = 0;
    }

    return ret;
}

short remain_command(command_queue *queue) {
    return queue->first != 0;
}

command_queue *command_parser (char *str) {
    char *buf_semi, *buf;
    char *saveptr1, *saveptr2;
    int argc;
    char *argv[MAX_ARGV];
    command_queue *queue;

    queue = create_queue();

    for (buf_semi = strtok_r(str, ";", &saveptr1); buf_semi;
	 buf_semi = strtok_r(0, ";", &saveptr1)) {
	argc = 0;
	for (buf = strtok_r(buf_semi, " ", &saveptr2); buf;
	     buf = strtok_r(0, " ", &saveptr2)) {
	    if (strcmp(buf, "&&") == 0) {
		add(queue, AND, create_command(argc, argv));
		argc = 0;
	    } else if (strcmp(buf, "||") == 0) {
		add(queue, OR, create_command(argc, argv));
		argc = 0;
	    } else if (strcmp(buf, ">") == 0) {
		add(queue, REDL, create_command(argc, argv));
		argc = 0;
	    } else if (strcmp(buf, "<") == 0) {
		add(queue, REDR, create_command(argc, argv));
		argc = 0;
	    } else {
		char* v = malloc(sizeof(buf));
		strcpy(v, buf);
		argv[argc++] = v;
	    }
	}

	if (argc != 0) add(queue, SEMI, create_command(argc, argv));
    }

    return queue;
}
