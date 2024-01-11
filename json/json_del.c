#include <ft_ssl.h>
#include <ssl_json.h>
#include <ssl_map.h>
#include <bnum.h>

void	__delete(t_node *);
void	__delete_array(t_node *);
void	__delete_object(t_node *);
void	__delete_string(t_node *);
void	__delete_number(t_node *);
void	__delete_boolean(t_node *);
void	__delete_null(t_node *);

void	json_del(t_node *node)
{
	if (NULL == node) {
		return ;
	}
	__delete(node);
}

void	__delete(t_node *node)
{
	if (node->type == JSON_CSTR) {
		__delete_string(node);
	} else if (node->type == JSON_NULL) {
		__delete_null(node);
	} else if (node->type == JSON_BOOLEAN) {
		__delete_boolean(node);
	} else if (node->type == JSON_NUMBER) {
		__delete_number(node);
	} else if (node->type == JSON_OBJECT) {
		__delete_object(node);
	} else if (node->type == JSON_ARRAY) {
		__delete_array(node);
	} else {
		LIBFT_FREE(node->key);
		LIBFT_FREE(node);
	}
}

void	__delete_array(t_node *node)
{
	ft_lst_del((t_node *)(node->content), __delete);
	LIBFT_FREE(node->key);
	LIBFT_FREE(node);
}

void	__delete_object(t_node *node)
{
	ft_htbl_del((t_htbl *)(node->content));
	LIBFT_FREE(node->key);
	LIBFT_FREE(node);
}

void	__delete_string(t_node *node)
{
	LIBFT_FREE(node->content);
	LIBFT_FREE(node->key);
	LIBFT_FREE(node);
}

void	__delete_null(t_node *node)
{
	LIBFT_FREE(node->key);
	LIBFT_FREE(node);
}

void	__delete_boolean(t_node *node)
{
	LIBFT_FREE(node->content);
	LIBFT_FREE(node->key);
	LIBFT_FREE(node);
}

void	__delete_number(t_node *node)
{
	bnum_del((t_num *)(node->content));
	LIBFT_FREE(node->key);
	LIBFT_FREE(node);
}