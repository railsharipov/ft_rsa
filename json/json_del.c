#include <libft/list.h>
#include <json.h>
#include <bnum.h>

static void __delete_array(void *content);
static void __delete_object(void *content);
static void __delete_string(void *content);
static void __delete_number(void *content);
static void __delete_boolean(void *content);
static void __delete_null(void *content);
static void __delete_bytes(void *content);

void json_del(t_node *node)
{
	(ft_node_del)(node);
}

t_func_content_del json_get_f_del(enum e_json_type type)
{
	if (type == JSON_TYPE_ARRAY) {
		return (__delete_array);

	} else if (type == JSON_TYPE_OBJECT) {
		return (__delete_object);

	} else if (type == JSON_TYPE_STRING) {
		return (__delete_string);

	} else if (type == JSON_TYPE_NULL) {
		return (__delete_null);

	} else if (type == JSON_TYPE_BOOL_TRUE) {
		return (__delete_boolean);

	} else if (type == JSON_TYPE_BOOL_FALSE) {
		return (__delete_boolean);

	} else if (type == JSON_TYPE_NUMBER) {
		return (__delete_number);
	}
	else if (type == JSON_TYPE_BYTES) {
		return (__delete_bytes);
	}
	return (NULL);
}

static void __delete_array(void *content)
{
	JSON_LOG(TRACE, "deleting array");
	ft_lst_del((t_node *)(content));
}

static void __delete_object(void *content)
{
	JSON_LOG(TRACE, "deleting object");
	ft_htbl_del((t_htbl *)(content));
}

static void __delete_string(void *content)
{
	JSON_LOG(TRACE, "deleting string");
	LIBFT_FREE(content);
}

static void __delete_null(void *content)
{
	JSON_LOG(TRACE, "deleting null");
	return ;
}

static void __delete_boolean(void *content)
{
	JSON_LOG(TRACE, "deleting boolean");
	return ;
}

static void __delete_number(void *content)
{
	JSON_LOG(TRACE, "deleting number");
	bnum_del((t_num *)(content));
}

static void __delete_bytes(void *content)
{
	JSON_LOG(TRACE, "deleting bytes");
	LIBFT_FREE(content);
}

