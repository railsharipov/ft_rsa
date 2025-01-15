#include <json.h>
#include <bnum.h>
#include <libft/htable.h>
#include <libft/alloc.h>
#include <libft/logger.h>
#include <libft/string.h>
#include <libft/list.h>

/*
**	VALUE	=	OBJ | ARR | STR | NUM | BOOL | NULL
**	NULL	=	null
**	BOOL	=	true | false
**	NUM		=	[0-9]+
**	STR		=	"[^"]*"
**	OBJ		=	"{" (KV ("," KV)*)? "}"
**	ARR		=	"[" (VALUE ("," VALUE)*)? "]"
**	KV		=	STR ":" VALUE
*/

enum	e_json_status
{
	JSON_MATCH = SSL_OK,
	JSON_NO_MATCH = SSL_STATUS_COUNT,
	JSON_BAD_FORMAT = SSL_STATUS_COUNT + 1,
};

typedef enum	e_json_value_type
{
	TYPE_OBJ,
	TYPE_ARR,
	TYPE_STR,
	TYPE_NUM,
	TYPE_BOOL,
	TYPE_NULL,
}				t_json_value_type;

typedef struct			s_json_value
{
	char				*content;
	size_t				size;
	t_json_value_type	type;
}						t_json_value;

static size_t	__pos;

static int	__parse_value(const char *s, t_node *node);
static int	__parse_null(const char *s, t_node *node);
static int	__parse_boolean(const char *s, t_node *node);
static int	__parse_number(const char *s, t_node *node);
static int	__parse_string(const char *s, t_node *node);
static int	__parse_kv(const char *s, t_node *node);
static int	__parse_object(const char *s, t_node *node);
static int	__parse_array(const char *s, t_node *node);
static void	__skip_ws(const char *s);

int json_parse_new(const char *s, t_node **node)
{
	t_node	*json_node;
	int 	status;

	if (s == NULL) {
		JSON_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	*node = NULL;
	__pos = 0;

	json_node = ft_node_create();
	status = __parse_value(s, json_node);

	if (status != JSON_MATCH) {
		JSON_LOG(ERROR, "bad format");
		return (SSL_ERR);
	}
	__skip_ws(s);

	if (s[__pos] != '\0') {
		JSON_LOG(ERROR, "bad format");
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static void	__skip_ws(const char *s)
{
	while (ft_iseolws(s[__pos]) && s[__pos] != '\0') {
		__pos++;
	}
}

static int	__parse_value(const char *s, t_node *node)
{
	int     status;

	__skip_ws(s);

	if (JSON_MATCH == (status = __parse_null(s, node))) {
		return (JSON_MATCH);
	} else if (JSON_BAD_FORMAT == status) {
		return (JSON_BAD_FORMAT);
	}

	if (JSON_MATCH == (status = __parse_boolean(s, node))) {
		return (JSON_MATCH);
	} else if (JSON_BAD_FORMAT == status) {
		return (JSON_BAD_FORMAT);
	}

	if (JSON_MATCH == (status = __parse_number(s, node))) {
		return (JSON_MATCH);
	} else if (JSON_BAD_FORMAT == status) {
		return (JSON_BAD_FORMAT);
	}

	if (JSON_MATCH == (status = __parse_string(s, node))) {
		return (JSON_MATCH);
	} else if (JSON_BAD_FORMAT == status) {
		return (JSON_BAD_FORMAT);
	}

	if (JSON_MATCH == (status = __parse_object(s, node))) {
		return (JSON_MATCH);
	} else if (JSON_BAD_FORMAT == status) {
		return (JSON_BAD_FORMAT);
	}

	if (JSON_MATCH == (status = __parse_array(s, node))) {
		return (JSON_MATCH);
	} else if (JSON_BAD_FORMAT == status) {
		return (JSON_BAD_FORMAT);
	}

	return (JSON_NO_MATCH);
}

static int	__parse_null(const char *s, t_node *node)
{
	size_t	old_pos;
	int 	status;

	old_pos = __pos;
	__skip_ws(s);

	JSON_LOG(TRACE, "parsing null at index %zu: %.20s...", __pos, s + __pos);

	if (ft_strncmp(s + __pos, "null", 4) == 0) {
		__pos += 4;
		return (JSON_MATCH);
	} else {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		__pos = old_pos;
		return (JSON_NO_MATCH);
	}
}

static int	__parse_boolean(const char *s, t_node *node)
{
	size_t	old_pos;
	int		status;

	old_pos = __pos;
	__skip_ws(s);

	JSON_LOG(TRACE, "parsing boolean at index %zu: %.20s...", __pos, s + __pos);

	if (ft_strncmp(s + __pos, "true", 4) == 0) {
		__pos += 4;
		return (JSON_MATCH);
	} else if (ft_strncmp(s + __pos, "false", 5) == 0) {
		__pos += 5;
		return (JSON_MATCH);
	} else {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		__pos = old_pos;
		return (JSON_NO_MATCH);
	}
}

static int	__parse_number(const char *s, t_node *node)
{
	size_t old_pos;
	int status;
	int is_negative;

	old_pos = __pos;
	__skip_ws(s);

	JSON_LOG(TRACE, "parsing number at index %zu: %.20s...", __pos, s + __pos);

	if (!ft_isdigit(s[__pos]) && s[__pos] != '-') {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		return (JSON_NO_MATCH);
	}

	if (s[__pos] == '-') {
		is_negative = 1;
		__pos++;
		if (!ft_isdigit(s[__pos])) {
			JSON_LOG(ERROR, "bad negative number format at index %d, %.20s...: expected digit, got '%c'", __pos, s + __pos, s[__pos]);
			__pos = old_pos;
			return (JSON_BAD_FORMAT);
		}
	}
	while (ft_isdigit(s[__pos])) {
		__pos++;
	}

	if (s[__pos] == '.') {
		__pos++;
		if (!ft_isdigit(s[__pos])) {
			JSON_LOG(ERROR, "bad float format at index %d, %.20s...: expected digit, got '%c'", __pos, s + __pos, s[__pos]);
			__pos = old_pos;
			return (JSON_BAD_FORMAT);
		}
		while (ft_isdigit(s[__pos])) {
			__pos++;
		}
	} else if (s[__pos] == 'e' || s[__pos] == 'E') {
		__pos++;
		if (s[__pos] == '-') {
			__pos++;
		}
		if (!ft_isdigit(s[__pos])) {
			JSON_LOG(ERROR, "bad float format at index %d, %.20s...: expected digit, got '%c'", __pos, s + __pos, s[__pos]);
			__pos = old_pos;
			return (JSON_BAD_FORMAT);
		}
		while (ft_isdigit(s[__pos])) {
			__pos++;
		}
	}
	return (JSON_MATCH);
}

static int	__parse_string(const char *s, t_node *node)
{
	size_t old_pos;
	int status;

	old_pos = __pos;
	__skip_ws(s);

	JSON_LOG(TRACE, "parsing string at index %zu: %.20s...", __pos, s + __pos);

	if (s[__pos] != '"') {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		return (JSON_NO_MATCH);
	}
	__pos++;
	while (s[__pos] != '"' && s[__pos] != '\0') {
		__pos++;
	}
	if (s[__pos] == '\0') {
		JSON_LOG(ERROR, "bad string format at index %d, %.20s...: expected '\"', got '%c'", __pos, s + __pos, s[__pos]);
		__pos = old_pos;
		return (JSON_BAD_FORMAT);
	}
	__pos++;
	return (JSON_MATCH);
}

static int	__parse_kv(const char *s, t_node *node)
{
	size_t old_pos;
	int status;

	old_pos = __pos;

	JSON_LOG(TRACE, "parsing key-value at index %zu: %.20s...", __pos, s + __pos);

	if (JSON_MATCH != (status = __parse_string(s, node))) {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		return (status);
	}
	__skip_ws(s);

	if (s[__pos] != ':') {
		JSON_LOG(ERROR, "bad key-value format at index %d, %.20s...: expected ':', got '%c'", __pos, s + __pos, s[__pos]);
		__pos = old_pos;
		return (JSON_BAD_FORMAT);
	}
	__pos++;

	if (JSON_MATCH != (status = __parse_value(s, node))) {
		JSON_LOG(ERROR, "bad key-value format at index %d, %.20s...: expected value", __pos, s + __pos);
		__pos = old_pos;
		return (JSON_BAD_FORMAT);
	}
	return (JSON_MATCH);
}

static int	__parse_object(const char *s, t_node *node)
{
	size_t old_pos;
	int status;

	old_pos = __pos;
	__skip_ws(s);

	JSON_LOG(TRACE, "parsing object at index %zu: %.20s...", __pos, s + __pos);

	if (s[__pos] != '{') {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		return (JSON_NO_MATCH);
	}
	__pos++;

	if (JSON_MATCH != (status = __parse_kv(s, node))) {
		JSON_LOG(ERROR, "bad object at index %d, %.20s...: expected key-value", __pos, s + __pos);
		__pos = old_pos;
		return (JSON_BAD_FORMAT);
	}
	__skip_ws(s);

	while (s[__pos] == ',') {
		__pos++;

		if (JSON_MATCH != (status = __parse_kv(s, node))) {
			JSON_LOG(ERROR, "bad object at index %d, %.20s...: expected key-value", __pos, s + __pos);
			__pos = old_pos;
			return (JSON_BAD_FORMAT);
		}
		__skip_ws(s);
	}
	if (s[__pos] != '}') {
		JSON_LOG(ERROR, "bad object format at index %d, %.20s...: expected '}', got '%c'", __pos, s + __pos, s[__pos]);
		__pos = old_pos;
		return (JSON_BAD_FORMAT);
	}
	__pos++;
	return (JSON_MATCH);
}

static int	__parse_array(const char *s, t_node *node)
{
	size_t old_pos;
	int status;

	old_pos = __pos;
	__skip_ws(s);

	JSON_LOG(TRACE, "parsing array at index %zu: %.20s...", __pos, s + __pos);

	if (s[__pos] != '[') {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		return (JSON_NO_MATCH);
	}
	__pos++;

	if (JSON_MATCH != (status = __parse_value(s, node))) {
		JSON_LOG(ERROR, "bad array format at index %d, %.20s...: expected value", __pos, s + __pos);
		__pos = old_pos;
		return (JSON_BAD_FORMAT);
	}
	__skip_ws(s);

	while (s[__pos] == ',') {
		__pos++;

		if (JSON_MATCH != (status = __parse_value(s, node))) {
			JSON_LOG(ERROR, "bad array format at index %d, %.20s...: expected value", __pos, s + __pos);
			__pos = old_pos;
			return (JSON_BAD_FORMAT);
		}
		__skip_ws(s);
	}
	if (s[__pos] != ']') {
		JSON_LOG(ERROR, "bad array format at index %d, %.20s...: expected ']', got '%c'", __pos, s + __pos, s[__pos]);
		__pos = old_pos;
		return (JSON_BAD_FORMAT);
	}
	__pos++;
	return (JSON_MATCH);
}
