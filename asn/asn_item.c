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
	if (NULL == item)
	{
		return ;
	}
	if (NULL != item->content)
	{
		ft_bzero(item->content, item->size);
		SSL_FREE(item->content);
	}
	SSL_FREE(item->type);
	SSL_FREE(item);
}

void	*asn_item_content(t_iasn *item)
{
	if (NULL == item)
	{
		return (NULL);
	}
	return (item->content);
}

size_t	asn_item_size(t_iasn *item)
{
	if (NULL == item)
	{
		return (0);
	}
	return (item->size);
}

char	*asn_item_type(t_iasn *item)
{
	if (NULL == item)
	{
		return (NULL);
	}
	return (item->type);
}
