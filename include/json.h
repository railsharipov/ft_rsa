#ifndef SSL_JSON_H
# define SSL_JSON_H

# include <common.h>
# include <libft.h>
# include <io.h>

typedef enum e_json_type {
    JSON_TYPE_BYTES = 1,
    JSON_TYPE_ARRAY,
    JSON_TYPE_OBJECT,
    JSON_TYPE_STRING,
    JSON_TYPE_NUMBER,
    JSON_TYPE_BOOL_TRUE,
    JSON_TYPE_BOOL_FALSE,
    JSON_TYPE_NULL,
	JSON_TYPE_COUNT
} t_json_type;

enum e_json_status {
	JSON_OK,
	JSON_ERR,
	JSON_FMT
};

enum e_json_q_type {
    JSON_Q_TYPE_OBJECT_KEY,
    JSON_Q_TYPE_ARRAY_INDEX,
    JSON_Q_TYPE_SELF,
};

enum e_json_q_status {
	JSON_MATCH_QUERY,
	JSON_NO_MATCH_QUERY,
	JSON_BAD_QUERY,
	JSON_BAD_QUERY_FORMAT,
};

typedef int (*t_func_json_map)(t_node *node);
typedef int (*t_func_json_select)(t_node *node, t_node *query_node, t_node **ret_node);
typedef int (*t_func_json_dump)(t_node *node, t_ostring *ostring);

int     json_parse(const char *s, t_node **node);
int		json_parse_file(const char *filename, t_node **node);
int		json_parse_stream(t_io_v2_stream *stream, t_node **node);
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
