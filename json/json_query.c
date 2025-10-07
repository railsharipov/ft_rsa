#include <json.h>
#include <bnum.h>
#include <libft/htable.h>
#include <libft/alloc.h>
#include <libft/logger.h>
#include <libft/string.h>
#include <libft/list.h>
#include <libft/tuple.h>

/*
**	QUERY ::= SEQUENCE {
**		selectors SEQUENCE OF SELECTOR
**	}
**
**	SELECTOR ::= CHOICE {
**		key KEY,
**		keyWithIndex SEQUENCE {
**			key KEY,
**			index INDEX
**		}
**	}
**
**	KEY ::= "." UTF8String (SIZE(1..MAX))
**
**	INDEX ::= "[" INTEGER (0..MAX) "]"
*/

#define __JQ_BAD_SELECTOR_ERROR	"bad selector"
#define __JQ_BAD_QUERY_ERROR	"bad query"
#define __JQ_BAD_TYPE_ERROR		"bad type"

static size_t	__pos;

static int 	__run_query(const char *s, t_node *json, t_node **ret_node, t_func_json_select f_selector);
static int 	__parse_selector(const char *s, t_node *query);
static int 	__f_default_selector(t_node *node, t_node *query, t_node **ret_node);
static int 	__select_object_key(t_node *node, t_node *query, t_node **ret_node);
static int 	__select_array_index(t_node *node, t_node *query, t_node **ret_node);

static const char	*__get_json_q_type_name(enum e_json_q_type type);

int json_query(const char *s, t_node *json, t_node **ret_node)
{
	return (json_query_with_f_selector(s, json, ret_node, __f_default_selector));
}

int json_query_with_f_selector(const char *s, t_node *json, t_node **ret_node, t_func_json_select f_selector)
{
	int ret;

	if (NULL == s) {
		JSON_LOG(ERROR, __JQ_BAD_QUERY_ERROR);
		return (JSON_BAD_QUERY);
	}

	if (NULL == json || NULL == ret_node) {
		JSON_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if (JSON_MATCH != (ret = __run_query(s, json, ret_node, f_selector))) {
		if (ret == JSON_NO_MATCH) {
			JSON_LOG(TRACE, "no such key: %s", s);
		} else {
			JSON_LOG(ERROR, __JQ_BAD_QUERY_ERROR);
		}
		return (SSL_ERR);
	}

	return (SSL_OK);
}

static int __run_query(const char *s, t_node *json, t_node **ret_node, t_func_json_select f_selector)
{
	t_node	*query;
	t_node	*cur_node;
	t_node  *selected_node;
	int		status;

	*ret_node = NULL;
	__pos = 0;
	cur_node = json;

	while (s[__pos] != '\0') {
		query = ft_node_create();
		selected_node = NULL;

		if (JSON_QUERY_OK == (status = __parse_selector(s, query))) {
			JSON_LOG(TRACE, "parsed %s", __get_json_q_type_name(query->type));
			status = f_selector(cur_node, query, &selected_node);
		} else {
			JSON_LOG(ERROR, "failed to parse query");
		}
		ft_node_del(query);

		if (status != JSON_MATCH) {
			return (status);
		}
		cur_node = selected_node;
	}
	*ret_node = selected_node;

	return (status);
}

static int 	__parse_selector(const char *s, t_node *query)
{
	size_t	begin, end;
	char	quote;

	if (s[__pos] == '.') {
		JSON_LOG(TRACE, "parsing object key");
		__pos++;
		begin = __pos;

		while (s[__pos] != '\0' && s[__pos] != '.' && s[__pos] != '[') {
			__pos++;
		}
		end = __pos;

		if (begin == end) {
			query->type = JSON_Q_SELF;
		} else {
			query->type = JSON_Q_OBJECT_KEY;
			query->content = ft_strsub(s, begin, end - begin);
		}
	}
	else if (s[__pos] == '[') {
		__pos++;

		if (ft_isdigit(s[__pos])) {
			JSON_LOG(TRACE, "parsing array index");
			begin = __pos;

			while (ft_isdigit(s[__pos])) {
				__pos++;
			}
			end = __pos;
			query->type = JSON_Q_ARRAY_INDEX;
		}
		else if (s[__pos] == '"' || s[__pos] == '\'') {
			JSON_LOG(TRACE, "parsing object key");
			quote = s[__pos];
			__pos++;
			begin = __pos;

			while (s[__pos] != '\0' && s[__pos] != quote) {
				__pos++;
			}

			if (s[__pos] != quote) {
				JSON_LOG(ERROR, __JQ_BAD_SELECTOR_ERROR ": expected terminating `%c`, got `%c`", quote, s[__pos]);
				return (JSON_BAD_QUERY);
			}
			end = __pos;
			__pos++;
			query->type = JSON_Q_OBJECT_KEY;
		}

		if (s[__pos] != ']') {
			JSON_LOG(ERROR, __JQ_BAD_SELECTOR_ERROR ": expected `]`, got `%c`", s[__pos]);
			return (JSON_BAD_QUERY);
		}
		__pos++;

		query->content = ft_strsub(s, begin, end - begin);
	}
	else {
		JSON_LOG(ERROR, __JQ_BAD_SELECTOR_ERROR ": `%c`", s[__pos]);
		return (JSON_BAD_QUERY);
	}

	return (JSON_QUERY_OK);
}

static int 	__f_default_selector(t_node *node, t_node *query, t_node **ret_node)
{
	if (query->type == JSON_Q_OBJECT_KEY) {
		return (__select_object_key(node, query, ret_node));
	}
	else if (query->type == JSON_Q_ARRAY_INDEX) {
		return (__select_array_index(node, query, ret_node));
	}
	else if (query->type == JSON_Q_SELF) {
		*ret_node = node;
		return (JSON_MATCH);
	}
	JSON_LOG(ERROR, "unknown query type");

	return (JSON_BAD_QUERY);
}

static int 	__select_object_key(t_node *node, t_node *query, t_node **ret_node)
{
	t_htbl	*htbl;
	t_node	*value_node;

	JSON_LOG(TRACE, "searching for object key: `%s`", query->content);

	if (node->type != JSON_TYPE_OBJECT) {
		JSON_LOG(ERROR, "using key for non-object type");
		return (JSON_BAD_FORMAT);
	}

	htbl = node->content;
	value_node = ft_htbl_get(htbl, query->content);

	if (value_node != NULL) {
		JSON_LOG(TRACE, "found node of type `%s`", json_get_type_name(value_node->type));
		*ret_node = value_node;
		return (JSON_MATCH);
	}

	JSON_LOG(TRACE, "no match found");
	return (JSON_NO_MATCH);
}

static int 	__select_array_index(t_node *node, t_node *query, t_node **ret_node)
{
	t_node	*arr_item;
	int		target_idx, idx;

	JSON_LOG(TRACE, "indexing array at: `%s`", query->content);

	if (node->type != JSON_TYPE_ARRAY) {
		JSON_LOG(ERROR, "using index for non-array type");
		return (JSON_BAD_FORMAT);
	}

	arr_item = node->content;
	target_idx = ft_atoi(query->content);

	JSON_LOG(TRACE, "array has %d items", ft_lst_size(arr_item));

	idx = 0;
	while (arr_item != NULL) {
		if (idx == target_idx) {
			JSON_LOG(TRACE, "found node of type `%s`", json_get_type_name(arr_item->type));
			*ret_node = arr_item;
			return (JSON_MATCH);
		}
		arr_item = arr_item->next;
		idx++;
	}

	JSON_LOG(TRACE, "no match found");
	return (JSON_NO_MATCH);
}

static const char	*__get_json_q_type_name(enum e_json_q_type type)
{
	switch (type) {
		case JSON_Q_OBJECT_KEY:
			return ("object key");
		case JSON_Q_ARRAY_INDEX:
			return ("array index");
		case JSON_Q_SELF:
			return ("self");
		default:
			return ("undefined");
	}
}
