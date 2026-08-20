#include <common.h>
#include <logger.h>
#include <json.h>
#include <bnum.h>
#include <libft.h>

#define __JSON_INVALID_JSON_ERROR	"invalid json"
#define __JSON_INVALID_NODE_ERROR	"invalid json node"
#define __JSON_INVALID_TYPE_ERROR	"invalid json type"

static int  __deep_scan;

static int	__validate_node(t_node *node);

static int	__validate_object(t_node *node);
static int	__validate_array(t_node *node);
static int	__validate_string(t_node *node);
static int	__validate_number(t_node *node);
static int	__validate_boolean(t_node *node);
static int	__validate_null(t_node *node);

int	json_validate(t_node *json)
{
	__deep_scan = 1;

	if (json == NULL) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	return (__validate_node(json));
}

int	json_validate_node(t_node *node)
{
	__deep_scan = 0;

	if (NULL == node) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	return (__validate_node(node));
}

int	json_validate_node_type(int type)
{
	__deep_scan = 0;

	if (type < 0 || type >= JSON_TYPE_COUNT) {
		SSL_LOG(DEBUG, __JSON_INVALID_TYPE_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

int	json_validate_node_is_of_type(t_node *node, int type)
{
	__deep_scan = 0;

	if (node->type != type) {
		SSL_LOG(DEBUG, "expected %s, got %s", json_get_type_name(type), json_get_type_name(node->type));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_node(t_node *node)
{
	if (NULL == node) {
		SSL_LOG(DEBUG, __JSON_INVALID_NODE_ERROR);
		return (SSL_ERR);
	}
	switch (node->type) {
		case JSON_TYPE_OBJECT:
			return (__validate_object(node));
		case JSON_TYPE_ARRAY:
			return (__validate_array(node));
		case JSON_TYPE_STRING:
			return (__validate_string(node));
		case JSON_TYPE_NUMBER:
			return (__validate_number(node));
		case JSON_TYPE_BOOL_TRUE:
		case JSON_TYPE_BOOL_FALSE:
			return (__validate_boolean(node));
		case JSON_TYPE_NULL:
			return (__validate_null(node));
		case JSON_TYPE_BYTES:
			return (SSL_OK);
		default:
			SSL_LOG(DEBUG, __JSON_INVALID_TYPE_ERROR);
			return (SSL_ERR);
	}
}

static int	__validate_object(t_node *node)
{
	t_htbl	*htbl;
	t_node	*item;
	t_node	*value_node;

	if (node->type != JSON_TYPE_OBJECT) {
		SSL_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_OBJECT), json_get_type_name(node->type));
		return (SSL_ERR);
	}

	if (__deep_scan) {
		htbl = node->content;
		item = NULL;

		while ((item = ft_htbl_node_next(htbl, item)) != NULL) {
			value_node = item->content;
			if (SSL_OK != __validate_node(value_node)) {
				SSL_LOG(DEBUG, "bad object kv: bad value for key `%s`", item->key);
				return (SSL_ERR);
			}
		}
	}

	return (SSL_OK);
}

static int	__validate_array(t_node *node)
{
	t_node	*arr_item;

	if (node->type != JSON_TYPE_ARRAY) {
		SSL_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_ARRAY), json_get_type_name(node->type));
		return (SSL_ERR);
	}

	if (__deep_scan) {
		arr_item = (t_node *)node->content;

		while (arr_item) {
			if (SSL_OK != __validate_node(arr_item)) {
				SSL_LOG(DEBUG, "%s: invalid array item", json_get_type_name(JSON_TYPE_ARRAY));
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
		SSL_LOG(DEBUG, "bad %s type", json_get_type_name(JSON_TYPE_STRING));
		return (SSL_ERR);
	}
	if (node->type != JSON_TYPE_STRING) {
		SSL_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_STRING), json_get_type_name(node->type));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_number(t_node *node)
{
	if (node->content == NULL) {
		SSL_LOG(DEBUG, "bad %s type", json_get_type_name(JSON_TYPE_NUMBER));
		return (SSL_ERR);
	}
	if (node->type != JSON_TYPE_NUMBER) {
		SSL_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_NUMBER), json_get_type_name(node->type));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_boolean(t_node *node)
{
	if (node->type != JSON_TYPE_BOOL_TRUE && node->type != JSON_TYPE_BOOL_FALSE) {
		SSL_LOG(DEBUG, "expected boolean type, got %s", json_get_type_name(node->type));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__validate_null(t_node *node)
{
	if (node->type != JSON_TYPE_NULL) {
		SSL_LOG(DEBUG, "expected %s, got %s", json_get_type_name(JSON_TYPE_NULL), json_get_type_name(node->type));
		return (SSL_ERR);
	}
	return (SSL_OK);
}
