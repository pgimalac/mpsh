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
FLAGS = -Wall -Wno-unused-function -Wextra -g $(foreach d, $(ALL_FOLDERS), -I $(d))

all: $(NAME)

%.c: %.l
	$(LEX) -t $? > $@

%.c: %.y
	$(YACC) -d $? -o $@

%.o: %.c
	$(CC) $(FLAGS) -c $? -o $@ $(LDLIBS) $(LDLIBS_OBJ)

$(NAME): $(DEPNAME)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME) $(LDLIBS)

clean:
	rm -rf $(OBJ) $(FILES_LP_FP)

fclean: clean
	rm -rf $(NAME)

re: fclean all
