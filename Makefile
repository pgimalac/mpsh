NAME = mpsh
FILES = $(wildcard src/*.c)
LDLIBS = -lreadline

OBJ = $(FILES:%.c=%.o)

CC = gcc
FLAGS = -Wall -Wextra -Werror -g

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(LDLIBS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(FLAGS) $(LDLIBS) -c $? -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all
