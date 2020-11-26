
INCLIB=/usr/local/lib

CC=gcc

CFLAGS= -Wall -Werror -Wextra

NAME= mlx-test
SRC = main.c
OBJ = $(SRC:.c=.o)

all	:$(NAME)

$(NAME)	:$(OBJ)
	$(CC) -o $(NAME) $(OBJ) -L$(INCLIB) -lmlx_Linux -lXext -lX11 -lm -lbsd

clean	:
	rm -f $(NAME) $(OBJ)


re	: clean all
