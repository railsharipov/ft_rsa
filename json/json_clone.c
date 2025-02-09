#include <common.h>
#include <json.h>
#include <bnum.h>
#include <libft/node.h>
#include <libft/string.h>
#include <libft/tuple.h>
#include <libft/list.h>

static void	__init_copy_node(t_node *src, t_node *dst);
static int	__copy_node(t_node *src, t_node *dst);
static int	__copy_node_of_type(t_node *src, t_node *dst, int type);
static int	__copy_kv_node(t_node *src, t_node *dst);

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

	__init_copy_node(src, dst);

	switch (type) {
		case JSON_TYPE_OBJECT:
			ret = __copy_object(src, dst);
			break;
		case JSON_TYPE_KV:
			ret = __copy_kv_node(src, dst);
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
		case JSON_TYPE_BOOLEAN:
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

static void	__init_copy_node(t_node *src, t_node *dst)
{
	ft_memcpy(dst, src, sizeof(t_node));
	dst->content = NULL;
	dst->size = 0;
}

static int	__copy_object(t_node *src, t_node *dst)
{
	t_node	*kv_arr_item;
	t_node	*dst_kv_list;

	dst_kv_list = NULL;
	kv_arr_item = (t_node *)src->content;

	while (kv_arr_item) {
		ft_lst_prepend(&dst_kv_list, ft_node_create());

		if (SSL_OK != __copy_node_of_type(kv_arr_item, dst_kv_list, JSON_TYPE_KV)) {
			ft_lst_del(dst_kv_list);
			return (SSL_ERR);
		}
		kv_arr_item = kv_arr_item->next;
	}

	ft_lst_rev(&dst_kv_list);
	dst->content = dst_kv_list;
	dst->size = src->size;

	return (SSL_OK);
}

static int	__copy_kv_node(t_node *src, t_node *dst)
{
	t_tuple	*tuple;
	t_node	*k, *v;
	t_node	*dst_k, *dst_v;

	tuple = (t_tuple *)src->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	dst_k = ft_node_create();

	if (SSL_OK != __copy_node_of_type(k, dst_k, JSON_TYPE_STRING)) {
		ft_node_del(dst_k);
		return (SSL_ERR);
	}

	dst_v = ft_node_create();

	if (SSL_OK != __copy_node(v, dst_v)) {
		ft_node_del(dst_v);
		return (SSL_ERR);
	}

	dst->content = ft_tuple_new(dst_k, sizeof(t_node), dst_v, sizeof(t_node));
	dst->size = src->size;

	return (SSL_OK);
}

static int	__copy_array(t_node *src, t_node *dst)
{
	t_node	*arr_item;
	t_node	*dst_list;

	dst_list = NULL;
	arr_item = (t_node *)src->content;

	while (arr_item) {
		ft_lst_prepend(&dst_list, ft_node_create());

		if (SSL_OK != __copy_node(arr_item, dst_list)) {
			ft_lst_del(dst_list);
			return (SSL_ERR);
		}
		arr_item = arr_item->next;
	}

	ft_lst_rev(&dst_list);
	dst->content = dst_list;
	dst->size = src->size;

	return (SSL_OK);
}

static int	__copy_string(t_node *src, t_node *dst)
{
	dst->content = ft_strdup(src->content);
	dst->size = src->size;

	return (SSL_OK);
}

static int	__copy_number(t_node *src, t_node *dst)
{
	dst->content = bnum_create();
	bnum_copy((t_num *)src->content, (t_num *)dst->content);
	dst->size = src->size;

	return (SSL_OK);
}

static int	__copy_boolean(t_node *src, t_node *dst)
{
	dst->content = ft_strdup(src->content);
	dst->size = src->size;

	return (SSL_OK);
}

static int	__copy_null(t_node *src, t_node *dst)
{
	(void)src;

	dst->content = NULL;
	dst->size = 0;

	return (SSL_OK);
}