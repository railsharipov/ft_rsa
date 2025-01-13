// #include <json.h>
// #include <bnum.h>
// #include <libft/htable.h>
// #include <libft/alloc.h>
// #include <libft/logger.h>
// #include <libft/string.h>
// #include <libft/list.h>

// enum e_select_type {
// 	JSON_SELECT_KEY = 0,
// 	JSON_SELECT_INDEX,
// 	JSON_INVALID_SELECTOR
// };

// struct	s_selector {
// 	enum e_select_type	type;
// 	char				*key;
// };

// struct t_node *__get_selectors(t_node *node, const char *s);
// ssize_t __get_object_by_key(t_node *node, const char *s, t_node **ret_node);
// ssize_t __get_object_by_selector(t_node *node, const char *s, t_node **ret_node);
// ssize_t __get_string_key(const char *s, char **ret);

// t_node *json_query(t_node *json_node, const char *s)
// {
// 	ssize_t	rbytes;
// 	t_node	*ret_node;
// 	t_node	*selectors;

// 	ret_node = NULL;

// 	if (NULL == json_node || NULL == s) {
// 		return (NULL);
// 	}

// 	selectors = __get_selectors(json_node, s);

// 	return (ret_node);
// }

// void	__f_del_selector(void *content) {
// 	struct s_selector *selector;

// 	selector = (struct s_selector *)content;

// 	if (NULL != selector) {
// 		SSL_FREE(selector->key);
// 		SSL_FREE(selector);
// 	}
// }

// struct t_node *__get_selectors(t_node *node, const char *s)
// {
// 	t_node	*selectors;
// 	struct s_selector	*selector;
// 	ssize_t	rbytes, tbytes;

// 	tbytes = 0;
// 	selectors = NULL;

// 	while (s[tbytes] != 0) {
// 		while (ft_iseolws(*s)) {
// 			tbytes++;
// 		}
// 		SSL_ALLOC(selector, sizeof(struct s_selector));

// 		rbytes = __get_selector(s + tbytes, selector);

// 		ft_lst_append(&selectors, ft_node_new(NULL, selector, sizeof(struct s_selector)));

// 		if (rbytes < 0) {
// 			break ;
// 		}
// 		tbytes += rbytes;
// 	}
// 	if (rbytes < 0) {
// 		ft_lst_del(selectors, __f_del_selector);
// 		return (NULL);
// 	}
// 	return (selectors);
// }

// ssize_t __get_selector(const char *s, struct s_selector *selector) {
// 	ssize_t	tbytes, rbytes;

// 	selector->key = NULL;
// 	selector->type = JSON_INVALID_SELECTOR;
// 	tbytes = 0;

// 	if (s[tbytes] == '.') {
// 		selector->type = JSON_SELECT_KEY;
// 		rbytes = __get_dot_key(s, &(selector->key));
// 	} else if (s[tbytes] == '[') {
// 		tbytes++;
// 		if (s[tbytes] == '"') {
// 			selector->type = JSON_SELECT_KEY;
// 			rbytes = __get_string_key(s, &(selector->key));
// 		} else {
// 			selector->type = JSON_SELECT_INDEX;
// 			rbytes = __get_index_key(s, &(selector->key));
// 		}
// 	} else {
// 		JSON_LOG(ERROR, "Invalid object selector");
// 		return (-1);
// 	}
// 	if (rbytes < 0) {
// 		return (-1);
// 	}
// 	tbytes += rbytes;
// 	return (tbytes);
// }

// ssize_t __get_dot_key(const char *s, char **ret) {
// 	ssize_t	tbytes, rbytes;

// 	tbytes = 1; // skip dot
// 	rbytes = __get_string_key(s + tbytes, &ret);

// 	if (rbytes < 0) {
// 		return (-1);
// 	}
// 	tbytes += rbytes;

// 	return (tbytes);
// }

// ssize_t __get_index_key(const char *s, char **ret) {
// 	ssize_t	tbytes, begin, rbytes;
// 	char	quote;

// 	tbytes = 1; // skip square bracket
// 	rbytes = __get_string_key(s + tbytes, &ret);

// 	if (rbytes < 0) {
// 		return (-1);
// 	}
// 	tbytes += rbytes;

// 	if (s[tbytes] != ']') {
// 		JSON_LOG(ERROR, "Expected matching `]` for index key");
// 		return (-1);
// 	} else {
// 		tbytes++;
// 	}

// 	return (tbytes);
// }

// ssize_t __get_string_key(const char *s, char **ret) {
// 	char	*key;
// 	ssize_t	idx;
// 	ssize_t	begin;
// 	char	quote;

// 	*ret = NULL;
// 	idx = 0;

// 	if (s[idx] == '"') {
// 		quote = s[idx++];
// 	} else {
// 		quote = 0;
// 	}
// 	begin = idx;

// 	while(ft_isalnum(s[idx])) {
// 		idx++;
// 	}
// 	key = ft_strsub(s, begin, idx);

// 	if (quote) {
// 		if (s[idx] == quote) {
// 			idx++;
// 		} else {
// 			JSON_LOG(ERROR, "Expected matching `%c` end of string for key: `%s`", quote, key);
// 			return (-1);
// 		}
// 	}
// 	*ret = key;
// 	return (idx);
// }

// ssize_t __get_object_by_key(t_node *node, const char *s, t_node **ret_node)
// {
// 	char	*key;
// 	ssize_t	idx;
// 	ssize_t	begin;
// 	t_htbl	*htbl;
// 	t_node	*target_node;
// 	char	quote;

// 	target_node = NULL;
// 	key = NULL;
// 	idx = 0;

// 	if (node->type != JSON_OBJECT) {
// 		JSON_LOG(ERROR, "Using key for non-object type");
// 		return (-1);
// 	}

// 	if (s[idx] == '"') {
// 		idx++;
// 		quote = s[idx];
// 	} else {
// 		quote = 0;
// 	}
// 	begin = idx;

// 	while(ft_isalnum(s[idx])) {
// 		idx++;
// 	}
// 	key = ft_strsub(s, begin, idx);

// 	if (quote) {
// 		if (s[idx] == quote) {
// 			idx++;
// 		} else {
// 			JSON_LOG(ERROR, "Expected matching `%c` end of string for key: `%s`", quote, key);
// 			goto err;
// 		}
// 	}

// 	htbl = (t_htbl *)node->content;
// 	target_node = (t_node *)ft_htbl_get(htbl, key);

// 	if (NULL == target_node) {
// 		JSON_LOG(ERROR, "No such key in object: `%s`", key);
// 		goto err;
// 	}

// 	SSL_FREE(key);
// 	*ret_node = target_node;
// 	return (idx);

// err:
// 	SSL_FREE(key);
// 	*ret_node = NULL;
// 	return (-1);
// }

// ssize_t __get_object_by_selector(t_node *node, const char *s, t_node **ret_node)
// {
// 	char 	*num_str;
// 	ssize_t idx;
// 	ssize_t begin;
// 	t_node	*lst;
// 	t_node 	*target_node;
// 	int		target_arr_idx;

// 	target_node = NULL;
// 	num_str = NULL;
// 	idx = 0;

// 	if (node->type != JSON_OBJECT) {
// 		JSON_LOG(ERROR, "Using key for non-object type");
// 		return (-1);
// 	}

// 	if (s[idx] == '[') {
// 		idx++;
// 	} else {
// 		return (-1);
// 	}

// 	while (ft_iseolws(s[idx])) {
// 		idx++;
// 	}

// 	if (s[idx] == '"') {
// 		return (__get_object_by_key(node, s + idx, &target_node));
// 	}
// 	begin = idx;

// 	while (ft_isdigit(s[idx])) {
// 		idx++;
// 	}
// 	num_str = ft_strsub(s, begin, idx);
// 	target_arr_idx = ft_atoi(num_str);

// 	while (ft_iseolws(s[idx])) {
// 		idx++;
// 	}

// 	if (s[idx] != ']'){
// 		JSON_LOG(ERROR, "Expected end of index selector for number: %s", num_str);
// 		goto err;
// 	}

// 	lst = (t_node *)node->content;

// 	if (ft_lst_size(lst) <= target_arr_idx) {
// 		JSON_LOG(ERROR, "Array index is out of bounds: %d", target_arr_idx);
// 		goto err;
// 	}

// 	while (target_arr_idx > 0) {
// 		target_arr_idx--;
// 		lst = lst->next;
// 	}

// 	SSL_FREE(num_str);
// 	*ret_node = lst;
// 	return (idx);

// err:
// 	SSL_FREE(num_str);
// 	*ret_node = NULL;
// 	return (-1);
// }