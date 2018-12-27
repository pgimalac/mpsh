NAME = mpsh
FOLDER_SRC = src/
FOLDER_LP = src/lp/
ALL_FOLDERS = $(FOLDER_SRC) $(FOLDER_LP)

FILES_SRC = env.c builtin.c \
		command.c completion.c \
		array.c list.c \
		hashmap.c hashset.c \
		parsing.c main.c
FILES_LP = lexer.c parser.c
FILES_YF = lexer.l parser.y

FILES_SRC_FP = $(addprefix $(FOLDER_SRC), $(FILES_SRC))
FILES_LP_FP = $(addprefix $(FOLDER_LP), $(FILES_LP))
FILES_YP_FP = $(addprefix $(FOLDER_LP), $(FILES_YF))

FILES = $(FILES_SRC_FP) $(FILES_LP_FP)

LDLIBS = -lreadline
LDLIBS_OBJ =

OBJ = $(FILES:%.c=%.o)

DEPNAME = $(FILES_YF_FP) $(OBJ)

CC = gcc
LEX = flex
YACC = bison
FLAGS = -Wall -Wno-unused-parameter -Wno-unused-function \
		-Wextra -g $(foreach d, $(ALL_FOLDERS), -I $(d))

all: gen $(NAME)

gen:
	$(LEX) -t src/lp/lexer.l > src/lp/lexer.c
	$(YACC) -d src/lp/parser.y -o src/lp/parser.c
	$(CC) $(FLAGS) -c src/lp/lexer.c -o src/lp/lexer.o $(LDLIBS) $(LDLIBS_OBJ)
	$(CC) $(FLAGS) -c src/lp/parser.c -o src/lp/parser.o $(LDLIBS) $(LDLIBS_OBJ)

%.o: %.c
	$(CC) $(FLAGS) -c $? -o $@ $(LDLIBS) $(LDLIBS_OBJ)

$(NAME): $(DEPNAME)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME) $(LDLIBS)

clean:
	rm -rf $(OBJ) $(FILES_LP_FP)

fclean: clean
	rm -rf $(NAME)

re: fclean all
