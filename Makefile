NAME = mpsh
FILES = src/types.c src/parsing.c src/lexer.c src/parser.c src/main.c
LDLIBS = -lreadline -ll

OBJ = $(FILES:%.c=%.o)

CC = gcc
FLAGS = -Wall -Wextra -g

all: $(NAME)

gen:
	lex -t src/lexer.l > src/lexer.c
	yacc -d src/parser.y -o src/parser.c

%.o: %.c
	$(CC) $(FLAGS) -c $? -o $@ $(LDLIBS)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME) -lreadline

clean:
	rm -rf $(OBJ)
	rm -rf src/lexer.c src/lexer.o
	rm -rf src/parser.h src/parser.c src/parser.o

fclean: clean
	rm -rf $(NAME)

re: fclean all
