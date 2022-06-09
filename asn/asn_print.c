#include <ft_ssl.h>
#include <ssl_asn.h>
#include <bnum.h>

static const int	INDENT_SPACE_CNT = 4;

static void	__print_space(int cnt)
{
	char	arr[cnt];

	ft_memset(arr, ' ', cnt);
	write(1, arr, cnt);
}

static void	__print_int_content(t_iasn *item, int space)
{
	char	*num_octets;
	int		numsize;
	int		idx;

	numsize = CEIL(lmbit_num(item->content), 8) / 8;

	if (numsize == 0)
	{
		numsize = 1;
	}
	num_octets = stringify_num(item->content);

	idx = 0;
	while (idx < numsize)
	{
		__print_space(space);
		util_puthex(num_octets+idx, MIN(15, numsize-idx), 0, ':');
		idx += 15;
	}
	SSL_FREE(num_octets);
}

static void	__print_content(t_iasn *item, int space)
{
	int	idx;

	idx = 0;
	while (idx < item->__size)
	{
		__print_space(space);
		util_puthex(item->content+idx, MIN(15, item->__size-idx), 0, ':');
		idx += 15;
	}
}

static void	__f_print(t_node *node, int space)
{
	t_iasn	*item;

	__print_space(space);

	if (NULL != node->key)
	{
		ft_printf("%s:\n", node->key);
	}
	else
	{
		ft_printf("node:\n");
	}
	if (NULL != node->content)
	{
		item = node->content;

		if (!ft_strcmp("int", item->__type))
		{
			__print_int_content(item, space + INDENT_SPACE_CNT);
		}
		else
		{
			__print_content(item, space + INDENT_SPACE_CNT);
		}
	}
}

void	asn_print(t_node *tree)
{
	if (NULL == tree)
	{
		return ;
	}
	ft_ntree_print(tree, __f_print);
}
