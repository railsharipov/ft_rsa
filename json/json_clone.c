#include <common.h>
#include <json.h>
#include <bnum.h>
#include <libft/node.h>
#include <libft/string.h>
#include <libft/tuple.h>
#include <libft/list.h>

static int	__copy_node(t_node *src, t_node *dst);
static int	__copy_node_of_type(t_node *src, t_node *dst, int type);

static int	__copy_object(t_node *src, t_node *dst);
static int	__copy_array(t_node *src, t_node *dst);
static int	__copy_string(t_node *src, t_node *dst);
static int	__copy_number(t_node *src, t_node *dst);
static int	__copy_boolean(t_node *src, t_node *dst);
static int	__copy_null(t_node *src, t_node *dst);

int	json_clone(t_node *json, t_node **ret_json)
{
	t_node	*node;

	if (json == NULL || ret_json == NULL) {
		JSON_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	*ret_json = NULL;
	node = ft_node_create();

	if (SSL_OK != __copy_node(json, node)) {
		ft_node_del(node);
		return (SSL_ERR);
	}

	*ret_json = node;
	return (SSL_OK);
}

static int	__copy_node(t_node *src, t_node *dst)
{
	return (__copy_node_of_type(src, dst, src->type));
}

static int	__copy_node_of_type(t_node *src, t_node *dst, int type)
{
	int		ret;

	if (NULL == src || NULL == dst) {
		JSON_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (src->type != type) {
		JSON_LOG(ERROR, "expected %s, got %s", json_get_type_name(type), json_get_type_name(src->type));
		return (SSL_ERR);
	}

	ft_node_init(dst);

	switch (type) {
		case JSON_TYPE_OBJECT:
			ret = __copy_object(src, dst);
			break;
		case JSON_TYPE_ARRAY:
			ret = __copy_array(src, dst);
			break;
		case JSON_TYPE_STRING:
			ret = __copy_string(src, dst);
			break;
		case JSON_TYPE_NUMBER:
			ret = __copy_number(src, dst);
			break;
		case JSON_TYPE_BOOL_TRUE:
		case JSON_TYPE_BOOL_FALSE:
			ret = __copy_boolean(src, dst);
			break;
		case JSON_TYPE_NULL:
			ret = __copy_null(src, dst);
			break;
		default:
			JSON_LOG(ERROR, "cannot clone type: %s", json_get_type_name(src->type));
			return (SSL_ERR);
	}

	return (ret);
}

static int	__copy_object(t_node *src, t_node *dst)
{
	t_htbl	*htbl;
	t_htbl	*dst_htbl;
	t_node	*item;
	t_node	*value_node;
	t_node	*dst_value_node;

	JSON_LOG(TRACE, "cloning object node");

	htbl = src->content;
	dst_htbl = ft_htbl_init(0);
	item = NULL;

	while ((item = ft_htbl_node_next(htbl, item)) != NULL) {
		value_node = item->content;
		dst_value_node = ft_node_create();

		if (SSL_OK != __copy_node(value_node, dst_value_node)) {
			ft_node_del(dst_value_node);
			ft_htbl_del(dst_htbl);
			return (SSL_ERR);
		}
		ft_htbl_add(dst_htbl, dst_value_node, item->key);
	}

	dst->type = JSON_TYPE_OBJECT;
	dst->content = dst_htbl;
	dst->size = 0;
	dst->f_del_content = json_get_f_del(dst->type);

	return (SSL_OK);
}

static int	__copy_array(t_node *src, t_node *dst)
{
	t_node	*src_list;
	t_node	*dst_list;
	t_node	*item;

	JSON_LOG(TRACE, "cloning array with %d items", src->size);

	dst_list = NULL;
	src_list = (t_node *)src->content;

	while (src_list) {
		item = ft_node_create();

		if (SSL_OK != __copy_node(src_list, item)) {
			ft_node_del(item);
			ft_lst_del(dst_list);
			return (SSL_ERR);
		}

		ft_lst_prepend(&dst_list, item);
		src_list = src_list->next;
	}

	ft_lst_rev(&dst_list);
	dst->type = JSON_TYPE_ARRAY;
	dst->content = dst_list;
	dst->size = ft_lst_size(dst_list);
	dst->f_del_content = json_get_f_del(src->type);

	return (SSL_OK);
}

static int	__copy_string(t_node *src, t_node *dst)
{
	JSON_LOG(TRACE, "cloning string: %s", src->content);

	dst->type = JSON_TYPE_STRING;
	dst->content = ft_strdup(src->content);
	dst->size = src->size;
	dst->f_del_content = json_get_f_del(src->type);

	return (SSL_OK);
}

static int	__copy_number(t_node *src, t_node *dst)
{
	t_num	*num;

	JSON_LOG(TRACE, "cloning number");

	num = bnum_create();
	bnum_copy((t_num *)src->content, num);

	dst->type = JSON_TYPE_NUMBER;
	dst->content = num;
	dst->size = 0;
	dst->f_del_content = json_get_f_del(src->type);

	return (SSL_OK);
}

static int	__copy_boolean(t_node *src, t_node *dst)
{
	JSON_LOG(TRACE, "cloning boolean: %s", src->content);

	dst->type = src->type;
	dst->content = NULL;
	dst->size = 0;
	dst->f_del_content = json_get_f_del(src->type);

	return (SSL_OK);
}

static int	__copy_null(t_node *src, t_node *dst)
{
	JSON_LOG(TRACE, "cloning null");

	(void)src;

	dst->type = JSON_TYPE_NULL;
	dst->content = NULL;
	dst->size = 0;
	dst->f_del_content = json_get_f_del(src->type);

	return (SSL_OK);
}