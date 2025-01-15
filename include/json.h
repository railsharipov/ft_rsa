#ifndef SSL_JSON_H
# define SSL_JSON_H

# include <common.h>
# include <libft/node.h>
# include <libft/std.h>
# include <libft/logger.h>

# define JSON_LOG(LEVEL, MES, ...)	json_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

enum e_json_type {
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_CSTR,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_NULL,
};

int		json_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int     json_parse(const char *s, t_node **node);
int     json_parse_new(const char *s, t_node **node);
void	json_del(t_node *node);

FUNC_CONTENT_DEL	json_get_f_del(enum e_json_type);

#endif