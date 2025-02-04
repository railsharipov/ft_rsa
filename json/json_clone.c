#include <common.h>
#include <json.h>
#include <bnum.h>
#include <libft/node.h>
#include <libft/string.h>
#include <libft/tuple.h>
#include <libft/list.h>

static void	__init_copy_node(t_node *src, t_node *dst);
static int	__copy_node(t_node *src, t_node *dst);
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

	return (__copy_node(json, node));
}

static int	__copy_node(t_node *src, t_node *dst)
{
	uint8_t	type;
	int		ret;

	if (NULL == src) {
		JSON_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	__init_copy_node(src, dst);

	switch (src->type) {
		case JSON_OBJECT:
			ret = __copy_object(src, dst);
			break;
		case JSON_ARRAY:
			ret = __copy_array(src, dst);
			break;
		case JSON_CSTR:
			ret = __copy_string(src, dst);
			break;
		case JSON_NUMBER:
			ret = __copy_number(src, dst);
			break;
		case JSON_BOOLEAN:
			ret = __copy_boolean(src, dst);
			break;
		case JSON_NULL:
			ret = __copy_null(src, dst);
			break;
		case JSON_UNKNOWN:
			JSON_LOG(ERROR, "invalid type: " JSON_TYPE_NAME_UNKNOWN);
			return (SSL_ERR);
		default:
			JSON_LOG(ERROR, "unknown type");
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
		if (kv_arr_item->type != JSON_KV) {
			JSON_LOG(ERROR, "Expected key-value pair");
			ft_lst_del(dst_kv_list);
			return (SSL_ERR);
		}
		ft_lst_prepend(&dst_kv_list, ft_node_create());

		if (SSL_OK != __copy_kv_node(kv_arr_item, dst_kv_list)) {
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
	k = tuple->head;
	v = tuple->tail;

	if (NULL == k || NULL == v) {
		JSON_LOG(ERROR, "Invalid key-value pair");
		return (SSL_ERR);
	}
	if (k->type != JSON_CSTR) {
		JSON_LOG(ERROR, "Expected key to be a string type");
		return (SSL_ERR);
	}

	dst_k = ft_node_create();

	if (SSL_OK != __copy_node(k, dst_k)) {
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