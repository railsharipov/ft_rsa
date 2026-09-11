#ifndef SSL_JSON_V2_H
# define SSL_JSON_V2_H

# include <common.h>
# include <libft.h>
# include <libft_v2.h>
# include <bnum.h>
# include <io.h>

typedef enum e_json_v2_type {
    JSON_V2_TYPE_NULL = 0,
    JSON_V2_TYPE_ARRAY,
    JSON_V2_TYPE_OBJECT,
    JSON_V2_TYPE_STRING,
    JSON_V2_TYPE_NUMBER,
    JSON_V2_TYPE_BOOL,
} t_json_v2_type;

enum e_json_v2_status {
	JSON_V2_OK,
	JSON_V2_ERR,
	JSON_V2_FMT
};

typedef struct s_json_v2_value {
	union {
		t_htbl_v2 htable;
		t_list    list;
		t_num     number;
		bool      boolean;
		char      *cstr;
	} as;
} t_json_v2_value;

typedef struct s_json_v2 {
	t_node_v2       base;
	t_json_v2_type  type;
	t_json_v2_value value;
} t_json_v2;

// enum e_json_q_type {
//     JSON_Q_TYPE_OBJECT_KEY,
//     JSON_Q_TYPE_ARRAY_INDEX,
//     JSON_Q_TYPE_SELF,
// };

// enum e_json_q_status {
// 	JSON_MATCH_QUERY,
// 	JSON_NO_MATCH_QUERY,
// 	JSON_BAD_QUERY,
// 	JSON_BAD_QUERY_FORMAT,
// };

typedef int (*t_func_json_v2_map)(t_json_v2 *json);
typedef int (*t_func_json_v2_select)(t_json_v2 *json, t_node *query_node, t_node **ret_node);
typedef void (*t_func_json_v2_dump)(t_json_v2 *json, t_ostring *ostring);

int	json_v2_parse(const char *s, t_json_v2 **json);
int	json_v2_parse_file(const char *filename, t_json_v2 **json);
int	json_v2_parse_stream(t_io_v2_stream *stream, t_json_v2 **json);

const char	*json_v2_get_type_name(t_json_v2_type type);

char	*json_v2_dumps(t_json_v2 *json);
char	*json_v2_dumps_with_f_dumper(t_json_v2 *json, t_func_json_v2_dump f_dumper);
size_t	json_v2_dumpb(t_json_v2 *json, char *buf, size_t size);
size_t	json_v2_dumpb_with_f_dumper(t_json_v2 *json, char *buf, size_t size, t_func_json_v2_dump f_dumper);

#endif
