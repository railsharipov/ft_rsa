#include <libft/list.h>
#include <json.h>
#include <bnum.h>

void __delete_array(void *content);
void __delete_object(void *content);
void __delete_string(void *content);
void __delete_number(void *content);
void __delete_boolean(void *content);
void __delete_null(void *content);

void json_del(t_node *node)
{
	(ft_node_del)(node);
}

FUNC_CONTENT_DEL json_get_f_del(enum e_json_type type)
{
	if (type == JSON_ARRAY) {
		return (__delete_array);

	} else if (type == JSON_OBJECT) {
		return (__delete_object);
		
	} else if (type == JSON_CSTR) {
		return (__delete_string);

	} else if (type == JSON_NULL) {
		return (__delete_null);

	} else if (type == JSON_BOOLEAN) {
		return (__delete_boolean);

	} else if (type == JSON_NUMBER) {
		return (__delete_number);
	}
	return (NULL);
}

void __delete_array(void *content)
{
	ft_lst_del((t_node *)(content), NULL);
}

void __delete_object(void *content)
{
	ft_htbl_del((t_htbl *)(content));
}

void __delete_string(void *content)
{
	LIBFT_FREE(content);
}

void __delete_null(void *content)
{
	return;
}

void __delete_boolean(void *content)
{
	LIBFT_FREE(content);
}

void __delete_number(void *content)
{
	bnum_del((t_num *)(content));
}