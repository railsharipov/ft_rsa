#include <ft_ssl.h>
#include <ssl_asn.h>
#include <bnum.h>

t_iasn	*asn_primitive_bool(const char *key, const char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_int(const char *key, const char *name)
{
	t_iasn	*item;
	size_t	size;

	item = asn_item_init();
	SSL_ALLOC(item->content, sizeof(t_num));
	init_num(item->content);
	size = sizeof(t_num);
	item->bitsize = NBYTES_TO_NBITS(size);
	item->type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_ostring(const char *key, const char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_bitstring(const char *key, const char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_null(const char *key, const char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_oid(const char *key, const char *name)
{
	t_iasn	*item;
	size_t	size;

	item = asn_item_init();
	item->type = ft_strdup(key);
	item->content = ft_strdup(name);
	size = ft_strlen(name);
	item->bitsize = NBYTES_TO_NBITS(size);

	return (item);
}
