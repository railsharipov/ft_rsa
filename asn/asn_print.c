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
	char	*octets;
	size_t	osize;
	size_t	idx;

	bnum_to_bytes_u(item->content, &octets, &osize);

	idx = 0;
	while (idx < osize)
	{
		__print_space(space);
		util_puthex(octets+idx, MIN(15, osize-idx), 0, ':');
		idx += 15;
	}
	SSL_FREE(octets);
}

static void	__print_content(t_iasn *item, int space)
{
	int	idx;

	idx = 0;
	while (idx < item->size)
	{
		__print_space(space);
		util_puthex(item->content+idx, MIN(15, item->size-idx), 0, ':');
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

		if (ASN_TAGNUM_INT == item->tagnum)
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
