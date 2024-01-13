# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/05/28 09:51:19 by rsharipo          #+#    #+#              #
#    Updated: 2018/10/12 18:55:47 by rsharipo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := ft_ssl

MODULES := ssl error bnum parser comm io rand hash base64 des asn der pem\
 rsa test json
LIBS := libft

OBJ_PREFIX := obj
DEP_PREFIX := dep

OBJ_DIRS := $(foreach dir, $(MODULES), $(addprefix $(OBJ_PREFIX)/,$(dir)))
OBJ_DIRS += $(foreach dir, $(LIBS), $(addprefix $(OBJ_PREFIX)/,$(dir)))

DEP_DIRS := $(foreach dir, $(MODULES), $(addprefix $(DEP_PREFIX)/,$(dir)))
DEP_DIRS += $(foreach dir, $(LIBS), $(addprefix $(DEP_PREFIX)/,$(dir)))

SRCS := $(foreach dir, $(MODULES), $(wildcard $(dir)/$(dir)_*.c))
SRCS += $(foreach dir, $(LIBS), $(wildcard $(dir)/*.c))

OBJS := $(foreach file, $(SRCS:.c=.o), $(addprefix $(OBJ_PREFIX)/,$(file)))

DEPS := $(foreach file, $(SRCS:.c=.d), $(addprefix $(DEP_PREFIX)/,$(file)))

CC = gcc -O3 -std=c11
CFLAGS = -I./include
LDFLAGS =
LDLIBS =
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_PREFIX)/$*.d

.PHONY: all sanitize debug

all: $(NAME)

debug: override CC = gcc -Og -g -fsanitize=address
debug: $(NAME)

$(NAME): $(OBJS)
	@echo linking: $(NAME)
	@$(CC) $(LDFLAGS) $(CFLAGS) $(LDLIBS) -o $@ $(OBJS)

$(OBJ_PREFIX)/%.o: %.c $(DEP_PREFIX)/%.d | $(OBJ_DIRS) $(DEP_DIRS)
	@echo compiling: $<
	@$(CC) $(DEPFLAGS) $(CFLAGS) $< -o $@ -c

$(OBJ_DIRS) $(DEP_DIRS):
	@mkdir -p $@

$(DEPS):
-include $(DEPS)

.PHONY: clean fclean re

clean:
	@echo cleaning
	-@rm -f $(OBJS) $(DEPS) || true
	-@rm -dR $(DEP_PREFIX) $(OBJ_PREFIX) || true

fclean: clean
	@echo removing $(NAME)
	-@rm -f $(NAME)

re: fclean
	make all
