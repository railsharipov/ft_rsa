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

enum __e_json_q_type {
	__JSON_Q_OBJECT = 0,
	__JSON_Q_ARRAY,
	__JSON_Q_SELF
};

static size_t	__pos;

int 	__run_query(const char *s, t_node *json, t_node **ret_node);
int 	__parse_selector(const char *s, t_node *query);
int 	__select_node(t_node *node, t_node *query, t_node **ret_node);
int 	__select_object(t_node *node, t_node *query, t_node **ret_node);
int 	__select_array(t_node *node, t_node *query, t_node **ret_node);

int json_query(const char *s, t_node *json, t_node **ret_node)
{
	if (NULL == s) {
		JSON_LOG(ERROR, __JQ_BAD_QUERY_ERROR);
		return (JSON_BAD_QUERY);
	}

	if (NULL == json || NULL == ret_node) {
		JSON_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if (JSON_MATCH != __run_query(s, json, ret_node)) {
		JSON_LOG(ERROR, __JQ_BAD_QUERY_ERROR);
		return (SSL_ERR);
	}

	return (SSL_OK);
}

int __run_query(const char *s, t_node *json, t_node **ret_node)
{
	t_node	*query;
	t_node	*json_node;
	int		status;

	__pos = 0;
	json_node = json;

	while (s[__pos] != '\0') {
		query = ft_node_create();
		*ret_node = NULL;

		if (JSON_MATCH == (status = __parse_selector(s, query))) {
			JSON_LOG(TRACE, "using query selector: %s", query->content);
			status = __select_node(json_node, query, ret_node);
		}

		ft_node_del(query);

		if (status != JSON_MATCH) {
			return (status);
		}
		json_node = *ret_node;
	}

	return (JSON_MATCH);
}

int __parse_selector(const char *s, t_node *query)
{
	size_t	begin, end;
	char	quote;
	if (s[__pos] == '.') {
		JSON_LOG(TRACE, "parsing object key selector");
		__pos++;
		begin = __pos;

		while (s[__pos] != '\0' && s[__pos] != '.' && s[__pos] != '[') {
			__pos++;
		}
		end = __pos;

		if (begin == end) {
			query->type = __JSON_Q_SELF;
		} else {
			query->type = __JSON_Q_OBJECT;
			query->content = ft_strsub(s, begin, end - begin);
		}
	}
	else if (s[__pos] == '[') {
		__pos++;

		if (ft_isdigit(s[__pos])) {
			JSON_LOG(TRACE, "parsing array selector");
			begin = __pos;

			while (ft_isdigit(s[__pos])) {
				__pos++;
			}
			end = __pos;
			query->type = __JSON_Q_ARRAY;
		}
		else if (s[__pos] == '"' || s[__pos] == '\'') {
			JSON_LOG(TRACE, "parsing object key selector");
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
			query->type = __JSON_Q_OBJECT;
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

	return (JSON_MATCH);
}

int 	__select_node(t_node *node, t_node *query, t_node **ret_node)
{
	if (query->type == __JSON_Q_OBJECT) {
		return (__select_object(node, query, ret_node));
	}
	else if (query->type == __JSON_Q_ARRAY) {
		return (__select_array(node, query, ret_node));
	}
	else if (query->type == __JSON_Q_SELF) {
		*ret_node = node;
		return (JSON_MATCH);
	}
	JSON_LOG(ERROR, "unknown query type");

	return (JSON_BAD_QUERY);
}

int 	__select_object(t_node *node, t_node *query, t_node **ret_node)
{
	t_node	*kv_node;
	t_node	*k, *v;
	t_tuple	*tuple;

	JSON_LOG(TRACE, "selecting object using key: %s", query->content);

	if (node->type != JSON_TYPE_OBJECT) {
		JSON_LOG(ERROR, "Using key for non-object type");
		return (JSON_BAD_FORMAT);
	}

	kv_node = node->content;

	while (kv_node != NULL) {
		if (SSL_OK != json_validate_node_is_of_type(kv_node, JSON_TYPE_KV)) {
			JSON_LOG(ERROR, __JQ_BAD_TYPE_ERROR);
			return (JSON_BAD_FORMAT);
		}
		tuple = (t_tuple *)kv_node->content;
		k = ft_tuple_get(tuple, 0);
		v = ft_tuple_get(tuple, 1);

		if (ft_strcmp(k->content, query->content) == 0) {
			JSON_LOG(TRACE, "found node of type %s", json_get_type_name(v->type));
			*ret_node = v;
			return (JSON_MATCH);
		}
		kv_node = kv_node->next;
	}

	JSON_LOG(TRACE, "no match found");
	return (JSON_NO_MATCH);
}

int 	__select_array(t_node *node, t_node *query, t_node **ret_node)
{
	t_node	*arr_item;
	int		target_idx, idx;

	JSON_LOG(TRACE, "selecting array using index: %s", query->content);

	if (node->type != JSON_TYPE_ARRAY) {
		JSON_LOG(ERROR, "Using index for non-array type");
		return (JSON_BAD_FORMAT);
	}

	arr_item = node->content;
	target_idx = ft_atoi(query->content);

	idx = 0;
	while (arr_item != NULL) {
		if (idx == target_idx) {
			JSON_LOG(TRACE, "found node of type %s", json_get_type_name(arr_item->type));
			*ret_node = arr_item;
			return (JSON_MATCH);
		}
		arr_item = arr_item->next;
		idx++;
	}

	JSON_LOG(TRACE, "no match found");
	return (JSON_NO_MATCH);
}
