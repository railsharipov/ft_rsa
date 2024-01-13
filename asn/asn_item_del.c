#include <ssl/ssl.h>
#include <ssl/asn.h>

void	asn_item_del(t_iasn *item)
{
	size_t	size;

	if (NULL == item)
		return ;

	if (NULL != item->content)
	{
		ft_bzero(item->content, item->size);
		item->size = 0;
		SSL_FREE(item->content);
	}

	SSL_FREE(item->description);
	SSL_FREE(item);
}
