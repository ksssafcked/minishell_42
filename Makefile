# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/14 13:31:37 by qudomino          #+#    #+#              #
#    Updated: 2024/12/20 16:54:22 by qudomino         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
SRC = src/main.c src/signals.c src/parser.c src/env.c utils/utils.c src/execute.c \
	  src/execute_pipeline.c src/heredoc.c src/parser_pipeline.c src/built_in.c \
	  src/expand.c src/parser_utils.c src/redirections.c 
OBJS = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -lreadline -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
