#include <ft_ssl.h>
#include <ssl_asn.h>
#include <bnum.h>

t_iasn	*asn_primitive_bool(const char *key, const char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->__type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_int(const char *key, const char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	SSL_ALLOC(item->content, sizeof(t_num));
	init_num(item->content);
	item->__size = sizeof(t_num);
	item->__type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_ostring(const char *key, const char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->__type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_bitstring(const char *key, const char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->__type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_null(const char *key, const char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->__type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_oid(const char *key, const char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->__type = ft_strdup(key);
	item->content = ft_strdup(name);
	item->__size = ft_strlen(name);

	return (item);
}
