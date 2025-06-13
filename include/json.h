#ifndef SSL_JSON_H
# define SSL_JSON_H

# include <common.h>
# include <libft/node.h>
# include <libft/std.h>
# include <libft/logger.h>

# define JSON_LOG_LEVEL				LIBFT_LOG_LEVEL_DEBUG

# define JSON_LOG(LEVEL, MES, ...)	json_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

enum	e_json_type {
    JSON_TYPE_UNDEFINED = 0,
    JSON_TYPE_ARRAY,
    JSON_TYPE_OBJECT,
    JSON_TYPE_KV,
    JSON_TYPE_STRING,
    JSON_TYPE_NUMBER,
    JSON_TYPE_BOOL_TRUE,
    JSON_TYPE_BOOL_FALSE,
    JSON_TYPE_NULL,
	JSON_TYPE_INVALID,
	JSON_TYPE_COUNT
};

enum	e_json_status
{
	JSON_MATCH 		= SSL_STATUS_COUNT,
	JSON_NO_MATCH	= SSL_STATUS_COUNT + 1,
	JSON_BAD_FORMAT = SSL_STATUS_COUNT + 2,
	JSON_BAD_TYPE	= SSL_STATUS_COUNT + 3,
	JSON_QUERY_OK	= SSL_STATUS_COUNT + 4,
	JSON_BAD_QUERY	= SSL_STATUS_COUNT + 5,
};

typedef int (*FUNC_JSON_MAP)(t_node *node);

int		json_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int     json_parse(const char *s, t_node **node);
int     json_query(const char *s, t_node *json, t_node **ret_node);
int     json_validate(t_node *node);
int     json_validate_node(t_node *node);
int     json_validate_node_type(int type);
int     json_validate_node_is_of_type(t_node *node, int type);
int     json_map(t_node *node, FUNC_JSON_MAP f, t_node **ret_node);
int     json_clone(t_node *node, t_node **ret_node);
void	json_del(t_node *node);

const char	*json_get_type_name(int type);

FUNC_CONTENT_DEL	json_get_f_del(enum e_json_type);

#endif