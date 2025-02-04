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

int 	__parse_selector(const char *s, t_node *query);
int 	__select_node(t_node *node, t_node *query, t_node **ret_node);
int 	__select_object(t_node *node, t_node *query, t_node **ret_node);
int 	__select_array(t_node *node, t_node *query, t_node **ret_node);

int json_query(const char *s, t_node *json, t_node **ret_node)
{
	t_node	*query;
	int		status;

	if (NULL == s) {
		JSON_LOG(ERROR, __JQ_BAD_QUERY_ERROR);
		return (JSON_BAD_QUERY);
	}

	if (NULL == json || NULL == ret_node) {
		JSON_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	*ret_node = NULL;
	__pos = 0;

	while (s[__pos] != '\0') {
		query = ft_node_create();

		if (JSON_MATCH == (status = __parse_selector(s, query))) {
			status = __select_node(json, query, ret_node);
		}

		ft_node_del(query);

		if (status == JSON_BAD_QUERY) {
			JSON_LOG(ERROR, __JQ_BAD_QUERY_ERROR);
			return (SSL_ERR);
		}
	}

	return (SSL_OK);
}

int __parse_selector(const char *s, t_node *query)
{
	size_t	begin, end;

	if (s[__pos] == '.') {
		__pos++;
		begin = __pos;

		while (ft_isprint(s[__pos])) {
			__pos++;
		}
		end = __pos;

		if (begin == end) {
			query->type = __JSON_Q_SELF;
		} else {
			query->type = __JSON_Q_OBJECT;
			query->key = ft_strsub(s, begin, end);
		}
	}
	else if (s[__pos] == '[') {
		__pos++;
		begin = __pos;

		while (ft_isdigit(s[__pos])) {
			__pos++;
		}
		end = __pos;

		if (s[__pos] != ']') {
			JSON_LOG(ERROR, __JQ_BAD_SELECTOR_ERROR ": expected `]`, got `%c`", s[__pos]);
			return (JSON_BAD_QUERY);
		}
		__pos++;

		query->type = __JSON_Q_ARRAY;
		query->key = ft_strsub(s, begin, end);
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

	if (node->type != JSON_OBJECT) {
		JSON_LOG(ERROR, "Using key for non-object type");
		return (JSON_BAD_FORMAT);
	}

	kv_node = node->content;

	while (kv_node != NULL) {
		if (SSL_OK != json_validate(kv_node)) {
			JSON_LOG(ERROR, __JQ_BAD_TYPE_ERROR);
			return (JSON_BAD_FORMAT);
		}
		tuple = (t_tuple *)kv_node->content;
		k = ft_tuple_get(tuple, 0);
		v = ft_tuple_get(tuple, 1);

		if (ft_strcmp(k->key, query->key) == 0) {
			*ret_node = v;
			return (JSON_MATCH);
		}
		kv_node = kv_node->next;
	}

	JSON_LOG(ERROR, __JQ_BAD_QUERY_ERROR);
	return (JSON_BAD_FORMAT);
}

int 	__select_array(t_node *node, t_node *query, t_node **ret_node)
{
	t_node	*arr_item;
	int		target_idx, idx;

	if (node->type != JSON_ARRAY) {
		JSON_LOG(ERROR, "Using index for non-array type");
		return (JSON_BAD_FORMAT);
	}

	arr_item = node->content;
	target_idx = ft_atoi(query->key);

	idx = 0;
	while (arr_item != NULL) {
		if (idx == target_idx) {
			*ret_node = arr_item;
			return (JSON_MATCH);
		}
		arr_item = arr_item->next;
		idx++;
	}

	JSON_LOG(ERROR, __JQ_BAD_QUERY_ERROR);
	return (JSON_BAD_FORMAT);
}
