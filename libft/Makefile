# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/05/28 09:51:19 by rsharipo          #+#    #+#              #
#    Updated: 2019/03/19 15:43:57 by rsharipo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

vpath	%.h	../include

NAME := libft.a

OBJ_DIR := obj

SRCS := $(wildcard ft_*.c)
OBJS := $(foreach file, $(SRCS:.c=.o), $(addprefix $(OBJ_DIR)/,$(file)))

CC = gcc -Wall -Wextra -Werror -Wno-incompatible-pointer-types \
	 -O3 -funroll-loops -fomit-frame-pointer
CFLAGS = -I../include
LDFLAGS =

.PHONY: all
all: $(NAME)

debug: override CFLAGS += -g -fsanitize=address
debug: $(NAME)

$(NAME): $(OBJS)
	@ar rc $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@echo compiling: $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR):
	@mkdir -p $@

$(OBJS): libft.h

.PHONY: clean
clean:
	-@rm -f $(OBJS) || true
	-@rm -dR $(OBJ_DIR) || true

.PHONY: fclean
fclean: clean
	-@rm -f $(NAME) || true

.PHONY: re
re: fclean
	make all
