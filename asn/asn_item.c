#include <ft_ssl.h>
#include <ssl_asn.h>

static char		*__get_type(t_iasn *item);
static size_t	__get_size(t_iasn *item);

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
		ft_bzero(item->content, item->__size);
		SSL_FREE(item->content);
	}
	SSL_FREE(item->__type);
	SSL_FREE(item);
}

size_t	__get_size(t_iasn *item)
{
	if (NULL == item)
	{
		return (0);
	}
	return (item->__size);
}

char	*__get_type(t_iasn *item)
{
	if (NULL == item)
	{
		return (NULL);
	}
	return (item->__type);
}
