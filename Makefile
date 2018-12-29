NAME = mpsh
FOLDER_SRC = src/
FOLDER_TYPES = src/types/
FOLDER_LP = src/lp/
ALL_FOLDERS = $(FOLDER_SRC) $(FOLDER_LP) $(FOLDER_TYPES)

FILES_SRC = utils.c env.c builtin.c \
			command.c completion.c \
			parsing.c main.c
FILES_TYPES = array.c list.c \
			  hashmap.c hashset.c
FILES_LP = lexer.c parser.c
FILES_YF = lexer.l parser.y

FILES_SRC_FP = $(addprefix $(FOLDER_SRC), $(FILES_SRC))
FILES_TYPES_FP = $(addprefix $(FOLDER_TYPES), $(FILES_TYPES))
FILES_LP_FP = $(addprefix $(FOLDER_LP), $(FILES_LP))
FILES_YP_FP = $(addprefix $(FOLDER_LP), $(FILES_YF))

FILES = $(FILES_TYPES_FP) $(FILES_SRC_FP) $(FILES_LP_FP)

LDLIBS = -lreadline
LDLIBS_OBJ =

OBJ = $(FILES:%.c=%.o)
DEPNAME = $(OBJ)

CC = gcc
LEX = flex
YACC = bison
FLAGS = -Wall -Wno-unused-parameter -Wno-unused-function \
		-Wextra -g $(foreach d, $(ALL_FOLDERS), -I $(d))

all: gen $(NAME)

gen:
	@echo "Generating lexer"
	@$(LEX) -t src/lp/lexer.l > src/lp/lexer.c
	@echo "Lexer generated in src/lp/lexer.c"
	@echo "Generating parser"
	@$(YACC) -d src/lp/parser.y -o src/lp/parser.c
	@echo "Parser generated in src/lp/parser.c"
	@echo "Start compiling objects"
	@echo "Compiling lexer"
	@$(CC) $(FLAGS) -c src/lp/lexer.c -o src/lp/lexer.o $(LDLIBS) $(LDLIBS_OBJ)
	@echo "Compiling parser"
	@$(CC) $(FLAGS) -c src/lp/parser.c -o src/lp/parser.o $(LDLIBS) $(LDLIBS_OBJ)

%.o: %.c
	@echo "Compiling $?"
	@$(CC) $(FLAGS) -c $? -o $@ $(LDLIBS) $(LDLIBS_OBJ)

$(NAME): $(DEPNAME)
	@echo "Build excutable $(NAME)"
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME) $(LDLIBS)
	@echo "Done."
	@cat ico

clean:
	@echo "Clean objects"
	@rm -rf $(OBJ) $(FILES_LP_FP)

fclean: clean
	@echo "Clean executable $(NAME)"
	@rm -rf $(NAME)

re: fclean all
