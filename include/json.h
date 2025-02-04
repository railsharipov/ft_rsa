#ifndef SSL_JSON_H
# define SSL_JSON_H

# include <common.h>
# include <libft/node.h>
# include <libft/std.h>
# include <libft/logger.h>

# define JSON_LOG(LEVEL, MES, ...)	json_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

# define JSON_TYPE_NAME_OBJECT	"object"
# define JSON_TYPE_NAME_ARRAY	"array"
# define JSON_TYPE_NAME_KV		"key-value"
# define JSON_TYPE_NAME_CSTR	"string"
# define JSON_TYPE_NAME_NUMBER	"number"
# define JSON_TYPE_NAME_BOOLEAN	"boolean"
# define JSON_TYPE_NAME_NULL	"null"
# define JSON_TYPE_NAME_UNKNOWN	"unknown type"

enum	e_json_type {
    JSON_ARRAY = 0,
    JSON_OBJECT,
    JSON_KV,
    JSON_CSTR,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_NULL,
    JSON_UNKNOWN,
	JSON_TYPE_COUNT
};

enum	e_json_status
{
	JSON_MATCH 		= SSL_STATUS_COUNT,
	JSON_NO_MATCH	= SSL_STATUS_COUNT + 1,
	JSON_BAD_FORMAT = SSL_STATUS_COUNT + 2,
	JSON_BAD_TYPE	= SSL_STATUS_COUNT + 3,
	JSON_BAD_QUERY	= SSL_STATUS_COUNT + 4,
};

typedef int (*FUNC_JSON_MAP)(t_node *node);

int		json_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int     json_parse(const char *s, t_node **node);
int     json_query(const char *s, t_node *json, t_node **ret_node);
int     json_validate(t_node *node);
int     json_map(t_node *node, FUNC_JSON_MAP f, t_node **ret_node);
int     json_clone(t_node *node, t_node **ret_node);
void	json_del(t_node *node);

FUNC_CONTENT_DEL	json_get_f_del(enum e_json_type);

#endif