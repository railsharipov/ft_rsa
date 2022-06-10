#include <ft_ssl.h>
#include <ssl_asn.h>

t_iasn	*asn_item_init(void)
{
	t_iasn	*item;

	SSL_ALLOC(item, sizeof(t_iasn));
	return (item);
}

void	asn_item_del(t_iasn *item)
{
	size_t	size;

	if (NULL == item)
	{
		return ;
	}
	if (NULL != item->content)
	{
		size = TO_NUM_BYTES(item->bitsize);
		ft_bzero(item->content, size);
		item->bitsize = 0;
		SSL_FREE(item->content);
	}
	if (NULL != item->type)
	{
		SSL_FREE(item->type);
	}
	SSL_FREE(item);
}
