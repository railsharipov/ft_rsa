#include <ft_ssl.h>
#include <ssl_asn.h>

t_iasn	*asn_construct_sequence(const char *key, const char *content)
{
	t_iasn	*item;

	item = asn_item_init();
	item->__type = ft_strdup(key);
	return (item);
}
