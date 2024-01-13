#ifndef SSL_JSON_H
# define SSL_JSON_H

# include <libft/node.h>
# include <libft/std.h>

# define JSON_ERROR(MES)	ft_printf("%@%s, %s:%d: %s\n", __func__, __FILE__, __LINE__, MES);

enum e_json_error
{
	INVALID_JSON_TYPE = 1,
	PARSE_JSON_FAILED,
	INVALID_FORMAT,
	UNEXPECTED_END_OF_CONTENT,
	UNEXPECTED_CHARS_AT_THE_END,
};

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