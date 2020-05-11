#include <ft_ssl.h>
#include <ssl_asn.h>
#include <bnum.h>

t_iasn	*asn_primitive_bool(char *key, char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_int(char *key, char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	SSL_ALLOC(item->content, sizeof(t_num));
	init_num(item->content);
	item->size = sizeof(t_num);
	item->type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_ostring(char *key, char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_bitstring(char *key, char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_null(char *key, char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->type = ft_strdup(key);
	return (item);
}

t_iasn	*asn_primitive_oid(char *key, char *name)
{
	t_iasn	*item;

	item = asn_item_init();
	item->type = ft_strdup(key);
	item->content = ft_strdup(name);
	item->size = ft_strlen(name);

	return (item);
}
