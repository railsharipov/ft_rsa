#include <common.h>
#include <json.h>
#include <bnum.h>
#include <libft/node.h>
#include <libft/string.h>
#include <libft/tuple.h>
#include <libft/list.h>

#define __JSON_INVALID_JSON_ERROR	"invalid json"
#define __JSON_INVALID_NODE_ERROR	"invalid json node"
#define __JSON_INVALID_TYPE_ERROR	"invalid json type"

static int  __deep_scan;

static int	__validate_node(t_node *node);

static int	__validate_object(t_node *node);
static int	__validate_kv(t_node *node);
static int	__validate_array(t_node *node);
static int	__validate_string(t_node *node);
static int	__validate_number(t_node *node);
static int	__validate_boolean(t_node *node);
static int	__validate_null(t_node *node);

int	json_validate(t_node *json)
{
	__deep_scan = 1;

	if (json == NULL) {
		JSON_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	return (__validate_node(json));
}

int	json_validate_node(t_node *node)
{
	__deep_scan = 0;

	if (NULL == node) {
		JSON_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	return (__validate_node(node));
}

int	json_validate_node_type(int type)
{
	__deep_scan = 0;

	if (type < 0 || type >= JSON_TYPE_COUNT) {
		JSON_LOG(DEBUG, __JSON_INVALID_TYPE_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

int	json_validate_node_is_of_type(t_node *node, int type)
{
	__deep_scan = 0;

	if (node->type != type) {
		JSON_LOG(DEBUG, "expected %s, got %s", json_get_type_name(type), json_get_type_name(node->type));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_node(t_node *node)
{
	if (NULL == node) {
		JSON_LOG(DEBUG, __JSON_INVALID_NODE_ERROR);
		return (SSL_ERR);
	}
	switch (node->type) {
		case JSON_TYPE_OBJECT:
			return (__validate_object(node));
		case JSON_TYPE_KV:
			return (__validate_kv(node));
		case JSON_TYPE_ARRAY:
			return (__validate_array(node));
		case JSON_TYPE_STRING:
			return (__validate_string(node));
		case JSON_TYPE_NUMBER:
			return (__validate_number(node));
		case JSON_TYPE_BOOLEAN:
			return (__validate_boolean(node));
		case JSON_TYPE_NULL:
			return (__validate_null(node));
		case JSON_TYPE_UNDEFINED:
			JSON_LOG(DEBUG, "unexpected type: %s", json_get_type_name(JSON_TYPE_UNDEFINED));
			return (SSL_ERR);
		default:
			JSON_LOG(DEBUG, __JSON_INVALID_TYPE_ERROR);
			return (SSL_ERR);
	}
}

static int	__validate_object(t_node *node)
{
	t_node	*kv_arr_item;

	if (node->type != JSON_TYPE_OBJECT) {
		JSON_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_OBJECT), json_get_type_name(node->type));
		return (SSL_ERR);
	}

	if (__deep_scan) {
		kv_arr_item = (t_node *)node->content;

		while (kv_arr_item) {
			if (SSL_OK != __validate_kv(kv_arr_item)) {
				JSON_LOG(DEBUG, "bad %s type", json_get_type_name(JSON_TYPE_KV));
				return (SSL_ERR);
			}
			kv_arr_item = kv_arr_item->next;
		}
	}

	return (SSL_OK);
}

static int	__validate_kv(t_node *node)
{
	t_tuple	*tuple;
	t_node	*k, *v;

	if (node->content == NULL) {
		JSON_LOG(DEBUG, "bad %s type", json_get_type_name(JSON_TYPE_KV));
		return (SSL_ERR);
	}
	if (node->type != JSON_TYPE_KV) {
		JSON_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_KV), json_get_type_name(node->type));
		return (SSL_ERR);
	}

	if (__deep_scan) {
		tuple = (t_tuple *)node->content;
		k = ft_tuple_get(tuple, 0);
		v = ft_tuple_get(tuple, 1);

		if (NULL == k || SSL_OK != __validate_string(k)) {
			JSON_LOG(DEBUG, "%s: expected key", json_get_type_name(JSON_TYPE_KV));
			return (SSL_ERR);
		}
		if (NULL == v || SSL_OK != __validate_node(v)) {
			JSON_LOG(DEBUG, "%s: expected value", json_get_type_name(JSON_TYPE_KV));
			return (SSL_ERR);
		}
	}

	return (SSL_OK);
}

static int	__validate_array(t_node *node)
{
	t_node	*arr_item;

	if (node->type != JSON_TYPE_ARRAY) {
		JSON_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_ARRAY), json_get_type_name(node->type));
		return (SSL_ERR);
	}

	if (__deep_scan) {
		arr_item = (t_node *)node->content;

		while (arr_item) {
			if (SSL_OK != __validate_node(arr_item)) {
				JSON_LOG(DEBUG, "%s: invalid array item", json_get_type_name(JSON_TYPE_ARRAY));
				return (SSL_ERR);
			}
			arr_item = arr_item->next;
		}
	}

	return (SSL_OK);
}

static int	__validate_string(t_node *node)
{
	if (node->content == NULL) {
		JSON_LOG(DEBUG, "bad %s type", json_get_type_name(JSON_TYPE_STRING));
		return (SSL_ERR);
	}
	if (node->type != JSON_TYPE_STRING) {
		JSON_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_STRING), json_get_type_name(node->type));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_number(t_node *node)
{
	if (node->content == NULL) {
		JSON_LOG(DEBUG, "bad %s type", json_get_type_name(JSON_TYPE_NUMBER));
		return (SSL_ERR);
	}
	if (node->type != JSON_TYPE_NUMBER) {
		JSON_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_NUMBER), json_get_type_name(node->type));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_boolean(t_node *node)
{
	if (node->content == NULL) {
		JSON_LOG(DEBUG, "bad %s type", json_get_type_name(JSON_TYPE_BOOLEAN));
		return (SSL_ERR);
	}
	if (node->type != JSON_TYPE_BOOLEAN) {
		JSON_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_BOOLEAN), json_get_type_name(node->type));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_null(t_node *node)
{
	if (NULL == node) {
		JSON_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (node->type != JSON_TYPE_NULL) {
		JSON_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_NULL), json_get_type_name(node->type));
		return (SSL_ERR);
	}
	return (SSL_OK);
}
