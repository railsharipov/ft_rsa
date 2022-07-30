#include <ft_ssl.h>
#include <ssl_asn.h>

t_iasn	*asn_item_dup(t_iasn *item)
{
	t_iasn	*dup_item;

	if (NULL == item)
		return (NULL);

	dup_item = asn_item_init();
	ft_memcpy(dup_item, item, sizeof(t_iasn));

	dup_item->content = ft_memdup(item->content, item->size);

	return (dup_item);
}
