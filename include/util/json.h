#ifndef SSL_JSON_H
# define SSL_JSON_H

# include <libft/node.h>
# include <libft/std.h>
# include <ssl/ssl.h>

# define JSON_ERROR(MES, ...)	ssl_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_ERROR, "json: ", MES __VA_OPT__(,) __VA_ARGS__)
# define JSON_INFO(MES, ...)	ssl_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_INFO, "json: ", MES __VA_OPT__(,) __VA_ARGS__)
# define JSON_DEBUG(MES, ...)	ssl_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_DEBUG, "json: ", MES __VA_OPT__(,) __VA_ARGS__)

enum e_json_type {
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_CSTR,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_NULL,
};

int     json_parse(const char *s, t_node **node);
int     json_parse_new(const char *s, t_node **node);
void	json_del(t_node *node);

FUNC_CONTENT_DEL	json_get_f_del(enum e_json_type);

#endif