#include <ssl/ssl.h>
#include <ssl/asn.h>

t_iasn	*asn_item_init(void)
{
	t_iasn	*item;

	SSL_ALLOC(item, sizeof(t_iasn));
	ft_bzero(item, sizeof(t_iasn));
	return (item);
}
