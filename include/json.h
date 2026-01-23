#ifndef SSL_JSON_H
# define SSL_JSON_H

# include <common.h>
# include <libft/node.h>
# include <libft/std.h>
# include <libft/logger.h>

# define JSON_LOGGER_NAME	"json"
# define JSON_LOG(LEVEL, MES, ...)	ft_logger_log(__func__, __FILE__, __LINE__, NULL, JSON_LOGGER_NAME, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

enum	e_json_type {
    JSON_TYPE_BYTES = 1,
    JSON_TYPE_ARRAY,
    JSON_TYPE_OBJECT,
    JSON_TYPE_STRING,
    JSON_TYPE_NUMBER,
    JSON_TYPE_BOOL_TRUE,
    JSON_TYPE_BOOL_FALSE,
    JSON_TYPE_NULL,
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

enum    e_json_q_type
{
    JSON_Q_OBJECT_KEY = 1,
    JSON_Q_ARRAY_INDEX,
    JSON_Q_SELF,
    JSON_Q_TYPE_COUNT
};

typedef int (*t_func_json_map)(t_node *node);
typedef int (*t_func_json_select)(t_node *node, t_node *query_node, t_node **ret_node);
typedef int (*t_func_json_dump)(t_node *node, t_ostring *ostring);

int     json_parse(const char *s, t_node **node);
int     json_query(const char *s, t_node *json, t_node **ret_node);
int     json_query_with_f_selector(const char *s, t_node *json, t_node **ret_node, t_func_json_select f_selector);
int     json_validate(t_node *node);
int     json_validate_node(t_node *node);
int     json_validate_node_type(int type);
int     json_validate_node_is_of_type(t_node *node, int type);
int     json_map(t_node *node, t_func_json_map f, t_node **ret_node);
int     json_clone(t_node *node, t_node **ret_node);
void	json_del(t_node *node);

char	*json_dump(t_node *node);
char	*json_dumps(t_node *node);
size_t	json_dumpb(t_node *node, char *buf, size_t size);
char	*json_dump_with_f_dumper(t_node *node, t_func_json_dump f_dumper);
char	*json_dumps_with_f_dumper(t_node *node, t_func_json_dump f_dumper);
size_t	json_dumpb_with_f_dumper(t_node *node, char *buf, size_t size, t_func_json_dump f_dumper);

const char	*json_get_type_name(int type);
const char	*json_get_query_type_name(int type);

t_func_content_del	json_get_f_del(enum e_json_type);

#endif