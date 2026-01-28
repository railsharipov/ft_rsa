NAME := ft_ssl

SRCS := main.c $(sort $(wildcard */*.c))

OBJ_DIR := obj
DEP_DIR := dep

OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS := $(SRCS:%.c=$(DEP_DIR)/%.d)

CC := gcc
CFLAGS := -O3 -std=c11 -I./include
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

.PHONY: all debug sanitize test clean fclean re

all: $(NAME)

debug: CFLAGS := -Og -g -std=c11 -Wall -Werror -Wfatal-errors -I./include
debug: $(NAME)

sanitize: CFLAGS := -Og -g -std=c11 -Wall -Werror -Wfatal-errors -fsanitize=address -fno-omit-frame-pointer -I./include
sanitize: LDFLAGS := -fsanitize=address
sanitize: $(NAME)

test: CFLAGS := -Og -g -std=c11 -Wall -Wextra -I./include
test: $(NAME)

$(NAME): $(OBJS)
	@echo linking: $(NAME)
	@$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(OBJ_DIR)/%.o: %.c | $(DEP_DIR)/%.d
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*.d)
	@echo compiling: $<
	@$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@

$(DEPS):
-include $(DEPS)

clean:
	@echo cleaning
	@rm -rf $(OBJ_DIR) $(DEP_DIR)

fclean: clean
	@echo removing $(NAME)
	@rm -f $(NAME)

re: fclean all
