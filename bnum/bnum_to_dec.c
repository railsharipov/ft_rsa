#include <bnum.h>

/* Convert bignum to decimal number string, sign is ignored */

struct __s_digit_symbol {
	char	c;
};

t_node	*__get_decimal_digit_list(const t_num *num);
char	*__get_decimal_num_string(t_node *list);
int 	__f_del_node(t_node *);

char	*bnum_to_dec(const t_num *num)
{
	t_node		*list;
	char		*s;

	if (NULL == num) {
		return (NULL);
	}
	if (BNUM_ZERO(num)) {
		return (ft_strdup("0"));
	}
	list = __get_decimal_digit_list(num);
	s = __get_decimal_num_string(list);
	ft_lst_del(list, __f_del_node);

	return (s);
}

t_node	*__get_decimal_digit_list(const t_num *num) {
	t_node 		*list;
	t_num 		copy;
	uint64_t	dig;
	int			sign;
	struct __s_digit_symbol *sym;

	sign = BNUM_SIGN(num);
	bnum_init(&copy);
	bnum_copy(num, &copy);
	list = NULL;

	while (!BNUM_ZERO(&copy))
	{
		LIBFT_ALLOC(sym, sizeof(struct __s_digit_symbol));
		bnum_divmod_dig(&copy, 10u, &copy, &dig);
		sym->c = (char)(dig + 48u);
		ft_lst_prepend(&list, ft_node_new(NULL, sym, 0));
	}
	if (sign == BNUM_NEG)
	{
		LIBFT_ALLOC(sym, sizeof(struct __s_digit_symbol));
		sym->c = '-';
		ft_lst_prepend(&list, ft_node_new(NULL, sym, 0));
	}
	bnum_clear(&copy);

	return (list);
}

char	*__get_decimal_num_string(t_node *list) {
	t_node		*node;
	char		*s;
	size_t		slen;
	size_t		idx;

	slen = ft_lst_size(list);
	LIBFT_ALLOC(s, slen + 1);

	node = list;
	idx = 0;
	while (node != NULL && idx < slen) {
		s[idx++] = ((struct __s_digit_symbol *)(node->content))->c;
		node = node->next;
	}
	s[idx] = '\0';

	return (s);
}

int __f_del_node(t_node *node) {
	LIBFT_FREE(node->content);
	return (0);
}