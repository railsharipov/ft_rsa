#include <common.h>
#include <asn.h>

t_iasn *asn_item_create(void)
{
	t_iasn *item;

	SSL_ALLOC(item, sizeof(t_iasn));
	ft_bzero(item, sizeof(t_iasn));
	return (item);
}

void	asn_item_init(t_iasn *item)
{
	if (NULL == item) {
		return ;
	}

	if (NULL != item->content) {
		ft_bzero(item->content, item->size);
		item->size = 0;
		SSL_FREE(item->content);
	}

	SSL_FREE(item->description);
}

void asn_item_del(t_iasn *item)
{
	size_t size;

	if (NULL == item)
	{
		return;
	}

	if (NULL != item->content)
	{
		ft_bzero(item->content, item->size);
		item->size = 0;
		SSL_FREE(item->content);
	}

	SSL_FREE(item->description);
	SSL_FREE(item);
}

t_iasn *asn_item_dup(t_iasn *item)
{
	t_iasn *dup_item;

	if (NULL == item) {
		return (NULL);
	}

	dup_item = asn_item_create();
	ft_memcpy(dup_item, item, sizeof(t_iasn));

	dup_item->content = ft_memdup(item->content, item->size);

	return (dup_item);
}

int	asn_item_set_type(t_iasn *item, char *type_key)
{
	uint8_t	tagnum;

	if (NULL == type_key || NULL == item) {
		return (ASN_LOG(ERROR, INVALID_INPUT_ERROR));
	}

	if (ft_streq(type_key, ASN_TYPE_KEY_SEQUENCE)) {
		tagnum = ASN_TAGNUM_SEQUENCE;
	}
	else if (ft_streq(type_key, ASN_TYPE_KEY_OSTRING)) {
		tagnum = ASN_TAGNUM_OCTET_STRING;
	}
	else if (ft_streq(type_key, ASN_TYPE_KEY_BITSTRING)) {
		tagnum = ASN_TAGNUM_BIT_STRING;
	}
	else if (ft_streq(type_key, ASN_TYPE_KEY_OBJECTID)) {
		tagnum = ASN_TAGNUM_OBJECT_ID;
	}
	else if (ft_streq(type_key, ASN_TYPE_KEY_INT)) {
		tagnum = ASN_TAGNUM_INT;
	}
	else if (ft_streq(type_key, ASN_TYPE_KEY_NULL)) {
		tagnum = ASN_TAGNUM_NULL;
	}
	else if (ft_streq(type_key, ASN_TYPE_KEY_BOOL)) {
		tagnum = ASN_TAGNUM_BOOLEAN;
	}
	else {
		return (ASN_LOG(ERROR, INVALID_INPUT_ERROR));
	}
	item->tagnum = tagnum;

	return (SSL_OK);
}