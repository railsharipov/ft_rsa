#ifndef SSL_JSON_H
# define SSL_JSON_H

# include <libft/node.h>
# include <libft/std.h>
# include <ssl/ssl.h>

# define JSON_ERROR(MES, ...)	ssl_error_log(__func__, __FILE__, __LINE__, "json error: ", MES __VA_OPT__(,) __VA_ARGS__)

enum e_json_type{
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_CSTR,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_NULL,
};

int     json_parse(const char *s, t_node **node);
void	json_del(t_node *node);

#endif