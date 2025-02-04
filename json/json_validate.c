#include <common.h>
#include <json.h>
#include <bnum.h>
#include <libft/node.h>
#include <libft/string.h>
#include <libft/tuple.h>
#include <libft/list.h>

#define __JSON_INVALID_JSON_ERROR		"invalid json"
#define __JSON_INVALID_NODE_ERROR		"invalid json node"
#define __JSON_UNEXPECTED_TYPE_ERROR	"unexpected type"
#define __JSON_INVALID_TYPE_ERROR(TYPE)	"invalid json type: " TYPE

static const char	*__JSON_TYPE_NAME_MAP[] = {
	[JSON_OBJECT]	= JSON_TYPE_NAME_OBJECT,
	[JSON_ARRAY]	= JSON_TYPE_NAME_ARRAY,
	[JSON_CSTR]		= JSON_TYPE_NAME_CSTR,
	[JSON_NUMBER]	= JSON_TYPE_NAME_NUMBER,
	[JSON_BOOLEAN]	= JSON_TYPE_NAME_BOOLEAN,
	[JSON_NULL]		= JSON_TYPE_NAME_NULL,
	[JSON_UNKNOWN]	= JSON_TYPE_NAME_UNKNOWN,
};

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
		JSON_LOG(ERROR, __JSON_INVALID_NODE_ERROR);
		return (SSL_ERR);
	}
	return (__validate_node(node));
}

int	json_validate_node_is_type(t_node *node, enum e_json_type type)
{
	__deep_scan = 0;

	if (NULL == node) {
		JSON_LOG(ERROR, __JSON_INVALID_NODE_ERROR);
		return (SSL_ERR);
	}
	if (type < 0 || type >= JSON_TYPE_COUNT) {
		JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR("bad type"));
		return (SSL_ERR);
	}
	if (node->type != type) {
		JSON_LOG(ERROR, __JSON_UNEXPECTED_TYPE_ERROR ": expected %s, got %s", __JSON_TYPE_NAME_MAP[type], __JSON_TYPE_NAME_MAP[node->type]);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_node(t_node *node)
{
	switch (node->type) {
		case JSON_OBJECT:
			return (__validate_object(node));
		case JSON_KV:
			return (__validate_kv(node));
		case JSON_ARRAY:
			return (__validate_array(node));
		case JSON_CSTR:
			return (__validate_string(node));
		case JSON_NUMBER:
			return (__validate_number(node));
		case JSON_BOOLEAN:
			return (__validate_boolean(node));
		case JSON_NULL:
			return (__validate_null(node));
		case JSON_UNKNOWN:
			JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_UNKNOWN));
			return (SSL_ERR);
		default:
			JSON_LOG(ERROR, __JSON_INVALID_NODE_ERROR);
			return (SSL_ERR);
	}
}

static int	__validate_object(t_node *node)
{
	t_node	*kv_arr_item;

	if (NULL == node || node->content == NULL) {
		JSON_LOG(ERROR, __JSON_INVALID_NODE_ERROR);
		return (SSL_ERR);
	}
	if (node->size != sizeof(t_node) || node->type != JSON_OBJECT) {
		JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_OBJECT));
		return (SSL_ERR);
	}

	if (__deep_scan) {
		kv_arr_item = (t_node *)node->content;

		while (kv_arr_item) {
			if (kv_arr_item->type != JSON_KV) {
				JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_OBJECT) ": invalid key-value pair");
				return (SSL_ERR);
			}

			if (SSL_OK != __validate_kv(kv_arr_item)) {
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

	if (NULL == node || node->content == NULL) {
		JSON_LOG(ERROR, __JSON_INVALID_NODE_ERROR);
		return (SSL_ERR);
	}
	if (node->size != sizeof(t_tuple) || node->type != JSON_KV) {
		JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_KV));
		return (SSL_ERR);
	}

	if (__deep_scan) {
		tuple = (t_tuple *)node->content;
		k = ft_tuple_get(tuple, 0);
		v = ft_tuple_get(tuple, 1);

		if (NULL == k) {
			JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_KV) ": expected key");
			return (SSL_ERR);
		}
		if (NULL == v) {
			JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_KV) ": expected value");
			return (SSL_ERR);
		}
		if (k->type != JSON_CSTR) {
			JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_KV) ": expected key to be a string type");
			return (SSL_ERR);
		}

		if (SSL_OK != __validate_string(k)) {
			JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_KV) ": invalid key");
			return (SSL_ERR);
		}

		if (SSL_OK != __validate_node(v)) {
			JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_KV) ": invalid value");
			return (SSL_ERR);
		}
	}

	return (SSL_OK);
}

static int	__validate_array(t_node *node)
{
	t_node	*arr_item;

	if (NULL == node || node->content == NULL) {
		JSON_LOG(ERROR, __JSON_INVALID_NODE_ERROR);
		return (SSL_ERR);
	}
	if (node->size != sizeof(t_node) || node->type != JSON_ARRAY) {
		JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_ARRAY));
		return (SSL_ERR);
	}

	if (__deep_scan) {
		arr_item = (t_node *)node->content;

		while (arr_item) {
			if (SSL_OK != __validate_node(arr_item)) {
				JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_ARRAY) ": invalid array item");
				return (SSL_ERR);
			}
			arr_item = arr_item->next;
		}
	}

	return (SSL_OK);
}

static int	__validate_string(t_node *node)
{
	if (NULL == node || node->content == NULL) {
		JSON_LOG(ERROR, __JSON_INVALID_NODE_ERROR);
		return (SSL_ERR);
	}
	if (node->type != JSON_CSTR) {
		JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_CSTR));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_number(t_node *node)
{
	if (NULL == node || node->content == NULL) {
		JSON_LOG(ERROR, __JSON_INVALID_NODE_ERROR);
		return (SSL_ERR);
	}
	if (node->size != sizeof(t_num) || node->type != JSON_NUMBER) {
		JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_NUMBER));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_boolean(t_node *node)
{
	if (NULL == node || node->content == NULL) {
		JSON_LOG(ERROR, __JSON_INVALID_NODE_ERROR);
		return (SSL_ERR);
	}
	if (node->size != sizeof(t_num) || node->type != JSON_BOOLEAN) {
		JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_BOOLEAN));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_null(t_node *node)
{
	if (NULL == node) {
		JSON_LOG(ERROR, __JSON_INVALID_NODE_ERROR);
		return (SSL_ERR);
	}
	if (node->size != sizeof(t_num) || node->type != JSON_NULL || node->content != NULL) {
		JSON_LOG(ERROR, __JSON_INVALID_TYPE_ERROR(JSON_TYPE_NAME_NULL));
		return (SSL_ERR);
	}
	return (SSL_OK);
}
