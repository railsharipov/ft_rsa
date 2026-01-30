#include <common.h>
#include <json.h>
#include <bnum.h>
#include <libft/node.h>
#include <libft/string.h>
#include <libft/tuple.h>
#include <libft/list.h>

static int	__map_node(t_node *node, int type, t_func_json_map f, t_node **ret_node);

static int	__map_object_node(t_node *node, t_func_json_map f, t_node *ret_node);
static int	__map_array_node(t_node *node, t_func_json_map f, t_node *ret_node);
static int	__map_string_node(t_node *node, t_func_json_map f, t_node *ret_node);
static int	__map_number_node(t_node *node, t_func_json_map f, t_node *ret_node);
static int	__map_boolean_node(t_node *node, t_func_json_map f, t_node *ret_node);
static int	__map_null_node(t_node *node, t_func_json_map f, t_node *ret_node);
static int	__map_bytes_node(t_node *node, t_func_json_map f, t_node *ret_node);

int	json_map(t_node *json, t_func_json_map f, t_node **ret_json)
{
	if (json == NULL || f == NULL || ret_json == NULL) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if (SSL_OK != __map_node(json, json->type, f, ret_json)) {
		SSL_LOG(ERROR, "failed to map values");
		return (SSL_ERR);
	}

	return (SSL_OK);
}

static int	__map_node(t_node *node, int type, t_func_json_map f, t_node **ret_node)
{
	t_node	*result_node;
	int		ret;

	if (node->type != type) {
		SSL_LOG(ERROR, "expected %s, got %s", json_get_type_name(type), json_get_type_name(node->type));
		return (SSL_ERR);
	}

	*ret_node = NULL;
	result_node = ft_node_create();
	ft_bzero(result_node, sizeof(t_node));

	switch (type) {
		case JSON_TYPE_OBJECT:
			ret = __map_object_node(node, f, result_node);
			break;
		case JSON_TYPE_ARRAY:
			ret = __map_array_node(node, f, result_node);
			break;
		case JSON_TYPE_STRING:
			ret = __map_string_node(node, f, result_node);
			break;
		case JSON_TYPE_NUMBER:
			ret = __map_number_node(node, f, result_node);
			break;
		case JSON_TYPE_BOOL_TRUE:
		case JSON_TYPE_BOOL_FALSE:
			ret = __map_boolean_node(node, f, result_node);
			break;
		case JSON_TYPE_NULL:
			ret = __map_null_node(node, f, result_node);
			break;
		case JSON_TYPE_BYTES:
			ret = __map_bytes_node(node, f, result_node);
			break;
		default:
			SSL_LOG(ERROR, "cannot map over type: %s", json_get_type_name(node->type));
			return (SSL_ERR);
	}

	*ret_node = result_node;

	return (ret);
}

static int	__map_object_node(t_node *node, t_func_json_map f, t_node *result_node)
{
	t_htbl	*htbl;
	t_htbl	*dst_htbl;
	t_node	*item;
	t_node	*value_node;
	t_node	*dst_value_node;
	int		status;

	SSL_LOG(TRACE, "mapping object node");

	htbl = node->content;
	dst_htbl = ft_htbl_create(0);

	item = NULL;
	while ((item = ft_htbl_node_next(htbl, item)) != NULL) {
		value_node = item->content;
		dst_value_node = ft_node_create();

		SSL_LOG(TRACE, "mapping over node with key: `%s`", item->key);

		if (SSL_OK != __map_node(value_node, value_node->type, f, &dst_value_node)) {
			ft_node_del(dst_value_node);
			ft_htbl_del(dst_htbl);
			return (SSL_ERR);
		}
		ft_htbl_add(dst_htbl, dst_value_node, item->key);
	}

	result_node->type = JSON_TYPE_OBJECT;
	result_node->content = dst_htbl;
	result_node->size = 0;
	result_node->f_del_content = json_get_f_del(JSON_TYPE_OBJECT);

	status = f(result_node);

	return (status);
}

static int	__map_array_node(t_node *node, t_func_json_map f, t_node *result_node)
{
	t_node	*src_list;
	t_node	*dst_list;
	t_node	*item;

	SSL_LOG(TRACE, "mapping array with %d items", node->size);

	dst_list = NULL;
	src_list = (t_node *)node->content;

	while (src_list) {
		item = ft_node_create();

		SSL_LOG(TRACE, "mapping array item of type: %s", json_get_type_name(src_list->type));

		if (SSL_OK != __map_node(src_list, src_list->type, f, &item)) {
			ft_node_del(item);
			ft_lst_del(dst_list);
			return (SSL_ERR);
		}

		ft_lst_prepend(&dst_list, item);
		src_list = src_list->next;
	}

	ft_lst_rev(&dst_list);
	result_node->type = JSON_TYPE_ARRAY;
	result_node->content = dst_list;
	result_node->size = ft_lst_size(dst_list);
	result_node->f_del_content = json_get_f_del(JSON_TYPE_ARRAY);

	return (SSL_OK);
}

static int	__map_string_node(t_node *node, t_func_json_map f, t_node *result_node)
{
	result_node->type = node->type;
	result_node->content = ft_strdup(node->content);
	result_node->size = node->size;
	result_node->f_del_content = node->f_del_content;

	return (f(result_node));
}

static int	__map_number_node(t_node *node, t_func_json_map f, t_node *result_node)
{
	t_num	*copy;

	copy = bnum_create();
	bnum_copy((t_num *)node->content, copy);

	result_node->type = node->type;
	result_node->content = copy;
	result_node->size = node->size;
	result_node->f_del_content = node->f_del_content;

	return (f(result_node));
}

static int	__map_boolean_node(t_node *node, t_func_json_map f, t_node *result_node)
{
	result_node->type = node->type;
	result_node->content = NULL;
	result_node->size = 0;
	result_node->f_del_content = node->f_del_content;

	return (f(result_node));
}

static int	__map_null_node(t_node *node, t_func_json_map f, t_node *result_node)
{
	result_node->type = node->type;
	result_node->content = NULL;
	result_node->size = 0;
	result_node->f_del_content = node->f_del_content;

	return (f(result_node));
}

static int	__map_bytes_node(t_node *node, t_func_json_map f, t_node *result_node)
{
	result_node->type = node->type;
	result_node->content = ft_memdup(node->content, node->size);
	result_node->size = node->size;
	result_node->f_del_content = node->f_del_content;

	return (f(result_node));
}