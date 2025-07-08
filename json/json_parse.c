#include <json.h>
#include <bnum.h>
#include <libft/htable.h>
#include <libft/alloc.h>
#include <libft/logger.h>
#include <libft/string.h>
#include <libft/list.h>
#include <libft/tuple.h>
#include <libft/queue.h>

/*
** VALUE ::= CHOICE {
**     obj     OBJ,
**     arr     ARR,
**     str     STR,
**     num     NUM,
**     bool    BOOL,
**     null    NULL
** }
**
** NULL ::= NULL
**
** BOOL ::= BOOLEAN
**
** NUM ::= CHOICE {
**     integer         INTEGER,
**     float           REAL,
**     exponential     REAL
** }
**
** STR ::= UTF8String
**
** OBJ ::= SEQUENCE {
**     "{" kvs "}" WHERE kvs ::= KV *("," KV)
** }
**
** ARR ::= SEQUENCE {
**     "[" values "]" WHERE values ::= VALUE *("," VALUE)
** }
**
** KV ::= SEQUENCE {
**     key STR,
**     colon ":",
**     value VALUE
** }
*/

typedef int (*FUNC_JSON_PARSE)(const char *, t_node *);

static size_t	__pos;

static int	__parse_value(const char *s, t_node *node);
static int	__parse_null(const char *s, t_node *node);
static int	__parse_boolean(const char *s, t_node *node);
static int	__parse_number(const char *s, t_node *node);
static int	__parse_string(const char *s, t_node *node);
static int	__parse_kv(const char *s, t_htbl *htbl);
static int	__parse_object(const char *s, t_node *node);
static int	__parse_array(const char *s, t_node *node);
static void	__skip_ws(const char *s);
static void __init_node(t_node *node);

int json_parse(const char *s, t_node **node)
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
	__init_node(json_node);
	status = __parse_value(s, json_node);
	__skip_ws(s);

	if (status != JSON_MATCH || s[__pos] != '\0') {
		ft_node_del(json_node);
		JSON_LOG(ERROR, "bad format");
		return (SSL_ERR);
	}
	*node = json_node;

	return (SSL_OK);
}

static void	__skip_ws(const char *s)
{
	while (ft_iseolws(s[__pos]) && s[__pos] != '\0') {
		__pos++;
	}
}

static void	__init_node(t_node *node)
{
	node->type = JSON_TYPE_BYTES;
	node->content = NULL;
	node->size = 0;
	node->f_del_content = NULL;
}

static int	__parse_value(const char *s, t_node *node)
{
	FUNC_JSON_PARSE f_arr[] = {
		__parse_null,
		__parse_boolean,
		__parse_number,
		__parse_string,
		__parse_object,
		__parse_array,
	};
	const int	arr_size = sizeof(f_arr) / sizeof(f_arr[0]);
	int     	status;

	__skip_ws(s);

	for (int i = 0; i < arr_size; i++) {
		status = f_arr[i](s, node);

		if (JSON_MATCH == status) {
			return (JSON_MATCH);
		}
		else if (JSON_NO_MATCH == status) {
			continue ;
		}
		else {
			return (status);
		}
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

	__init_node(node);
	node->type = JSON_TYPE_NULL;
	node->f_del_content = json_get_f_del(JSON_TYPE_NULL);

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

	__init_node(node);

	if (ft_strncmp(s + __pos, "true", 4) == 0) {
		__pos += 4;
		node->type = JSON_TYPE_BOOL_TRUE;
		node->f_del_content = json_get_f_del(JSON_TYPE_BOOL_TRUE);
		return (JSON_MATCH);
	} else if (ft_strncmp(s + __pos, "false", 5) == 0) {
		__pos += 5;
		node->type = JSON_TYPE_BOOL_FALSE;
		node->f_del_content = json_get_f_del(JSON_TYPE_BOOL_FALSE);
		return (JSON_MATCH);
	} else {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		__pos = old_pos;
		return (JSON_NO_MATCH);
	}
}

static int	__parse_number(const char *s, t_node *node)
{
	size_t 		old_pos;
	size_t		mantissa_start, mantissa_end;
	size_t  	fraction_start, fraction_end;
	size_t		exponent_start, exponent_end;
	int 		is_neg_mantissa, is_neg_exponent;
	int 		is_float, is_exponent;
	int 		status;
	char		*mantissa_str, *exponent_str;
	t_num		*mantissa, *number;
	uint64_t	exponent;

	is_neg_mantissa = 0;
	is_neg_exponent = 0;
	is_float = 0;
	is_exponent = 0;

	old_pos = __pos;
	__skip_ws(s);

	JSON_LOG(TRACE, "parsing number at index %zu: %.20s...", __pos, s + __pos);

	__init_node(node);
	node->type = JSON_TYPE_NUMBER;
	node->f_del_content = json_get_f_del(JSON_TYPE_NUMBER);

	if (!ft_isdigit(s[__pos]) && s[__pos] != '-') {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		return (JSON_NO_MATCH);
	}

	if (s[__pos] == '-') {
		is_neg_mantissa = 1;
		__pos++;
		if (!ft_isdigit(s[__pos])) {
			JSON_LOG(ERROR, "bad negative number format at index %d, %.20s...: expected digit, got '%c'", __pos, s + __pos, s[__pos]);
			__pos = old_pos;
			return (JSON_BAD_FORMAT);
		}
	}

	mantissa_start = __pos;

	while (ft_isdigit(s[__pos])) {
		__pos++;
	}
	mantissa_end = __pos;

	if (s[__pos] == '.') {
		is_float = 1;
		__pos++;
		fraction_start = __pos;

		if (!ft_isdigit(s[__pos])) {
			JSON_LOG(ERROR, "bad float format at index %d, %.20s...: expected digit, got '%c'", __pos, s + __pos, s[__pos]);
			__pos = old_pos;
			return (JSON_BAD_FORMAT);
		}
		while (ft_isdigit(s[__pos])) {
			__pos++;
		}
		fraction_end = __pos;
	}

	if (s[__pos] == 'e' || s[__pos] == 'E') {
		is_exponent = 1;
		__pos++;
		exponent_start = __pos;

		if (s[__pos] == '-') {
			is_neg_exponent = 1;
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
		exponent_end = __pos;
	}

	if (is_float || is_neg_exponent) {
		JSON_LOG(ERROR, NOT_IMPLEMENTED_ERROR);
		return (JSON_BAD_FORMAT);
	}
	if (is_exponent) {
		exponent_str = ft_strsub(s, exponent_start, exponent_end - exponent_start);
		exponent = (uint64_t)ft_atoi(exponent_str);
		LIBFT_FREE(exponent_str);
	}

	mantissa_str = ft_strsub(s, mantissa_start, mantissa_end - mantissa_start);
	mantissa = bnum_from_dec(mantissa_str);
	LIBFT_FREE(mantissa_str);

	number = bnum_create();

	if (is_exponent) {
		bnum_exp(mantissa, exponent, number);
	} else {
		bnum_copy(mantissa, number);
	}
	if (is_neg_mantissa) {
		number->sign = BNUM_NEG;
	}
	bnum_del(mantissa);

	node->content = number;
	node->size = 0;

	return (JSON_MATCH);
}

static int	__parse_string(const char *s, t_node *node)
{
	size_t 	old_pos;
	size_t 	str_start, str_end;
	int 	status;

	old_pos = __pos;
	__skip_ws(s);

	JSON_LOG(TRACE, "parsing string at index %zu: %.20s...", __pos, s + __pos);

	__init_node(node);
	node->type = JSON_TYPE_STRING;
	node->f_del_content = json_get_f_del(JSON_TYPE_STRING);

	if (s[__pos] != '"') {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		return (JSON_NO_MATCH);
	}
	__pos++;
	str_start = __pos;

	while (s[__pos] != '"' && s[__pos] != '\0') {
		__pos++;
	}
	str_end = __pos;

	if (s[__pos] == '\0') {
		JSON_LOG(ERROR, "bad string format at index %d, %.20s...: expected '\"', got '%c'", __pos, s + __pos, s[__pos]);
		__pos = old_pos;
		return (JSON_BAD_FORMAT);
	}
	__pos++;

	node->content = ft_strsub(s, str_start, str_end - str_start);
	node->size = str_end - str_start;

	return (JSON_MATCH);
}

static int	__parse_kv(const char *s, t_htbl *htbl)
{
	t_node	*key_node, *value_node;
	size_t 	old_pos;
	int 	status;

	status = JSON_NO_MATCH;
	old_pos = __pos;

	JSON_LOG(TRACE, "parsing key-value at index %zu: %.20s...", __pos, s + __pos);

	key_node = ft_node_create();
	value_node = ft_node_create();

	if (JSON_MATCH != (status = __parse_string(s, key_node))) {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		goto label_exit;
	}
	__skip_ws(s);

	if (s[__pos] != ':') {
		JSON_LOG(ERROR, "bad key-value format at index %d, %.20s...: expected ':', got '%c'", __pos, s + __pos, s[__pos]);
		status = JSON_BAD_FORMAT;
		goto label_exit;
	}
	__pos++;

	if (JSON_MATCH != (status = __parse_value(s, value_node))) {
		JSON_LOG(ERROR, "bad key-value format at index %d, %.20s...: expected value", __pos, s + __pos);
		goto label_exit;
	}
	status = JSON_MATCH;

	ft_htbl_add_with_f_del(htbl, value_node, key_node->content, json_get_f_del(value_node->type));

label_exit:
	ft_node_del(key_node);

	if (status != JSON_MATCH) {
		__pos = old_pos;
		ft_node_del(value_node);
	}

	return (status);
}

static int	__parse_object(const char *s, t_node *node)
{
	t_htbl	*htbl;
	size_t  old_pos;
	int     status;

	old_pos = __pos;
	__skip_ws(s);

	JSON_LOG(TRACE, "parsing object at index %zu: %.20s...", __pos, s + __pos);

	__init_node(node);
	node->type = JSON_TYPE_OBJECT;
	node->f_del_content = json_get_f_del(JSON_TYPE_OBJECT);

	htbl = ft_htbl_create(0);

	if (s[__pos] != '{') {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		status = JSON_NO_MATCH;
		goto label_exit;
	}
	__pos++;

	if (s[__pos] == '}') {
		__pos++;
		status = JSON_MATCH;
		goto label_exit;
	}

	do {
		if (JSON_MATCH != (status = __parse_kv(s, htbl))) {
			JSON_LOG(ERROR, "bad object at index %d, %.20s...: expected key-value", __pos, s + __pos);
			goto label_exit;
		}
		__skip_ws(s);

	} while ((s[__pos] == ',') && (++__pos));

	if (s[__pos] != '}') {
		JSON_LOG(ERROR, "bad object format at index %d, %.20s...: expected '}', got '%c'", __pos, s + __pos, s[__pos]);
		status = JSON_BAD_FORMAT;
		goto label_exit;
	}
	__pos++;

	node->content = htbl;

	status = JSON_MATCH;

label_exit:
	if (status != JSON_MATCH) {
		__pos = old_pos;
		ft_htbl_del(htbl);
	}
	return (status);
}

static int	__parse_array(const char *s, t_node *node)
{
	t_node	*value_node_list;
	size_t	old_pos;
	int 	status;

	old_pos = __pos;
	__skip_ws(s);

	JSON_LOG(TRACE, "parsing array at index %zu: %.20s...", __pos, s + __pos);

	__init_node(node);
	node->type = JSON_TYPE_ARRAY;
	node->f_del_content = json_get_f_del(JSON_TYPE_ARRAY);

	value_node_list = NULL;

	if (s[__pos] != '[') {
		JSON_LOG(TRACE, "no match at index %zu: %c", __pos, s[__pos]);
		status = JSON_NO_MATCH;
		goto label_exit;
	}
	__pos++;

	ft_lst_prepend(&value_node_list, ft_node_create());

	if (JSON_MATCH != (status = __parse_value(s, value_node_list))) {
		JSON_LOG(ERROR, "bad array format at index %d, %.20s...: expected value", __pos, s + __pos);
		goto label_exit;
	}
	__skip_ws(s);

	while (s[__pos] == ',') {
		__pos++;

		ft_lst_prepend(&value_node_list, ft_node_create());

		if (JSON_MATCH != (status = __parse_value(s, value_node_list))) {
			JSON_LOG(ERROR, "bad array format at index %d, %.20s...: expected value", __pos, s + __pos);
			goto label_exit;
		}
		__skip_ws(s);
	}
	if (s[__pos] != ']') {
		JSON_LOG(ERROR, "bad array format at index %d, %.20s...: expected ']', got '%c'", __pos, s + __pos, s[__pos]);
		status = JSON_BAD_FORMAT;
		goto label_exit;
	}
	__pos++;

	ft_lst_rev(&value_node_list);

	node->content = value_node_list;
	node->size = ft_lst_size(value_node_list);

	status = JSON_MATCH;

label_exit:
	if (status != JSON_MATCH) {
		__pos = old_pos;
		ft_lst_del(value_node_list);
	}

	return (status);
}
