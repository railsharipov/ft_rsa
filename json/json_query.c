#include <util/json.h>
#include <util/bnum.h>
#include <libft/htable.h>
#include <libft/alloc.h>
#include <libft/error.h>
#include <libft/string.h>
#include <libft/list.h>

ssize_t __get_node(t_node *node, const char *s, t_node **ret_node);
ssize_t __get_object_by_key(t_node *node, const char *s, t_node **ret_node);
ssize_t __get_object_by_selector(t_node *node, const char *s, t_node **ret_node);

t_node *json_query(t_node *json_node, const char *s)
{
	ssize_t	rbytes;
	t_node	*ret_node;

	ret_node = NULL;

	if (NULL == json_node || NULL == s) {
		return (NULL);
	}

	while (ft_iseolws(*s)) {
		s++;
	}

	rbytes = __get_node(json_node, s, &ret_node);

	return (ret_node);
}

ssize_t __get_node(t_node *node, const char *s, t_node **ret_node)
{
	t_node	*target_node;
	ssize_t	rbytes;

	rbytes = 0;
	target_node = NULL;

	if (*s == '.') {
		return(__get_node(target_node, s + 1, ret_node));
	} else if (*s == '[') {
		rbytes = __get_object_by_selector(node, s, &target_node);
	} else {
		rbytes = __get_object_by_key(node, s, &target_node);
	}

	if (rbytes < 0) {
		return (-1);
	}

	while (ft_iseolws(s[rbytes])) {
		rbytes++;
	}
	if (s[rbytes] == 0) {
		*ret_node = target_node;
		return (rbytes);
	}

	return (__get_node(target_node, s + rbytes, ret_node));
}

ssize_t __get_object_by_key(t_node *node, const char *s, t_node **ret_node)
{
	char	*key;
	ssize_t	idx;
	ssize_t	begin;
	t_htbl	*htbl;
	t_node	*target_node;
	char	quote;

	target_node = NULL;
	key = NULL;
	idx = 0;

	if (node->type != JSON_OBJECT) {
		JSON_ERROR("Using key for non-object type");
		return (-1);
	}

	if (s[idx] == '"') {
		idx++;
		quote = s[idx];
	} else {
		quote = 0;
	}
	begin = idx;

	while(ft_isalnum(s[idx])) {
		idx++;
	}
	key = ft_strsub(s, begin, idx);

	if (quote) {
		if (s[idx] == quote) {
			idx++;
		} else {
			JSON_ERROR("Expected matching `%c` end of string for key: `%s`", quote, key);
			goto err;
		}
	}

	htbl = (t_htbl *)node->content;
	target_node = (t_node *)ft_htbl_get(htbl, key);

	if (NULL == target_node) {
		JSON_ERROR("No such key in object: `%s`", key);
		goto err;
	}

	SSL_FREE(key);
	*ret_node = target_node;
	return (idx);

err:
	SSL_FREE(key);
	*ret_node = NULL;
	return (-1);
}

ssize_t __get_object_by_selector(t_node *node, const char *s, t_node **ret_node)
{
	char 	*num_str;
	ssize_t idx;
	ssize_t begin;
	t_node	*lst;
	t_node 	*target_node;
	int		target_arr_idx;

	target_node = NULL;
	num_str = NULL;
	idx = 0;

	if (node->type != JSON_OBJECT) {
		JSON_ERROR("Using key for non-object type");
		return (-1);
	}

	if (s[idx] == '[') {
		idx++;
	} else {
		return (-1);
	}

	while (ft_iseolws(s[idx])) {
		idx++;
	}

	if (s[idx] == '"') {
		return (__get_object_by_key(node, s + idx, &target_node));
	}
	begin = idx;

	while (ft_isdigit(s[idx])) {
		idx++;
	}
	num_str = ft_strsub(s, begin, idx);
	target_arr_idx = ft_atoi(num_str);

	while (ft_iseolws(s[idx])) {
		idx++;
	}

	if (s[idx] != ']'){
		JSON_ERROR("Expected end of index selector for number: %s", num_str);
		goto err;
	}

	lst = (t_node *)node->content;

	if (ft_lst_size(lst) <= target_arr_idx) {
		JSON_ERROR("Array index is out of bounds: %d", target_arr_idx);
		goto err;
	}

	while (target_arr_idx > 0) {
		target_arr_idx--;
		lst = lst->next;
	}

	SSL_FREE(num_str);
	*ret_node = lst;
	return (idx);

err:
	SSL_FREE(num_str);
	*ret_node = NULL;
	return (-1);
}