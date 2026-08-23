NAME := ft_ssl
TEST_NAME := test

SRCS := main.c base64.c $(filter-out tests/%,$(sort $(wildcard */*.c)))
TEST_SRCS := base64.c $(filter-out main.c,$(sort $(wildcard */*.c)))

OBJ_DIR := obj
DEP_DIR := dep
TEST_OBJ_DIR := obj_test
TEST_DEP_DIR := dep_test

OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS := $(SRCS:%.c=$(DEP_DIR)/%.d)
TEST_OBJS := $(TEST_SRCS:%.c=$(TEST_OBJ_DIR)/%.o)
TEST_DEPS := $(TEST_SRCS:%.c=$(TEST_DEP_DIR)/%.d)

CC := gcc
CFLAGS := -O3 -std=c11 -I./include
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

.PHONY: all debug sanitize test sanitize_test test_bin clean fclean re

all: CFLAGS := -O3 -std=c11 -Wall -Wfatal-errors -I./include -I./libft
all: $(NAME)

debug: CFLAGS := -Og -g -std=c11 -Wall -Wfatal-errors -I./include -I./libft
debug: $(NAME)

sanitize: CFLAGS := -Og -g -std=c11 -Wall -Wfatal-errors -fsanitize=address -fno-omit-frame-pointer -I./include -I./libft
sanitize: LDFLAGS := -fsanitize=address
sanitize: $(NAME)

$(NAME): $(OBJS)
	@echo linking: $(NAME)
	@$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(OBJ_DIR)/%.o: %.c | $(DEP_DIR)/%.d
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*.d)
	@echo compiling: $<
	@$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.o: %.c | $(TEST_DEP_DIR)/%.d
	@mkdir -p $(dir $@) $(dir $(TEST_DEP_DIR)/$*.d)
	@echo compiling test: $<
	@$(CC) -MT $@ -MMD -MP -MF $(TEST_DEP_DIR)/$*.d $(CFLAGS) -c $< -o $@

-include $(DEPS)
-include $(TEST_DEPS)

$(DEPS):
$(TEST_DEPS):

sanitize_test: CFLAGS := -Og -g -std=c11 -Wall -Wfatal-errors -fsanitize=address -fno-omit-frame-pointer -I./include -I./libft
sanitize_test: LDFLAGS := -fsanitize=address
sanitize_test: test_bin

test: CFLAGS := -Og -g -std=c11 -Wall -Wfatal-errors -I./include -I./libft
test: test_bin

test_bin: $(TEST_OBJS)
	@echo linking: $(TEST_NAME)
	@$(CC) $(LDFLAGS) -o $(TEST_NAME) $(TEST_OBJS)

clean:
	@echo cleaning
	@rm -rf $(OBJ_DIR) $(DEP_DIR) $(TEST_OBJ_DIR) $(TEST_DEP_DIR)

fclean: clean
	@echo removing $(NAME) $(TEST_NAME)
	@rm -f $(NAME) $(TEST_NAME)

re: fclean all
