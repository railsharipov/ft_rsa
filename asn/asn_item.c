#include <common.h>
#include <logger.h>
#include <asn1.h>

t_iasn *asn1_item_create(void)
{
	t_iasn *item;

	SSL_ALLOC(item, sizeof(t_iasn));
	ft_bzero(item, sizeof(t_iasn));
	return (item);
}

void	asn1_item_init(t_iasn *item)
{
	if (NULL == item) {
		return ;
	}

	ft_bzero(item, sizeof(t_iasn));
}

void asn1_item_clear(t_iasn *item)
{
	if (NULL == item) {
		return;
	}

	SSL_FREE(item->content);
	SSL_FREE(item->description);
	ft_bzero(item, sizeof(t_iasn));
}

void asn1_item_del(t_iasn *item)
{
	if (NULL == item) {
		return;
	}

	SSL_FREE(item->content);
	SSL_FREE(item->description);
	SSL_FREE(item);
}

t_iasn *asn1_item_dup(t_iasn *item)
{
	t_iasn *dup_item;

	if (NULL == item) {
		return (NULL);
	}

	dup_item = asn1_item_create();
	ft_memcpy(dup_item, item, sizeof(t_iasn));

	dup_item->content = ft_memdup(item->content, item->size);

	return (dup_item);
}

int	asn1_item_set_type(t_iasn *item, char *type)
{
	uint8_t	tagnum;

	if (NULL == type || NULL == item) {
		return (SSL_LOG(ERROR, INVALID_INPUT_ERROR));
	}

	if (ft_streq(type, ASN_TYPE_NAME_SEQUENCE)) {
		tagnum = ASN_TAGNUM_SEQUENCE;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_OSTRING)) {
		tagnum = ASN_TAGNUM_OCTET_STRING;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_BITSTRING)) {
		tagnum = ASN_TAGNUM_BIT_STRING;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_OBJECT_ID)) {
		tagnum = ASN_TAGNUM_OBJECT_ID;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_OBJECT_DESC)) {
		tagnum = ASN_TAGNUM_OBJECT_DESCR;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_INT)) {
		tagnum = ASN_TAGNUM_INT;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_NULL)) {
		tagnum = ASN_TAGNUM_NULL;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_BOOL)) {
		tagnum = ASN_TAGNUM_BOOLEAN;
	}
	else {
		SSL_LOG(ERROR, "unknown asn type: %s", type);
		return (SSL_ERR);
	}
	item->tagnum = tagnum;

	return (SSL_OK);
}

char	*asn1_item_get_type_name(t_iasn *item)
{
	if (NULL == item) {
		return (NULL);
	}

	switch (item->tagnum) {
		case ASN_TAGNUM_SEQUENCE:
			return ASN_TYPE_NAME_SEQUENCE;
		case ASN_TAGNUM_OCTET_STRING:
			return ASN_TYPE_NAME_OSTRING;
		case ASN_TAGNUM_BIT_STRING:
			return ASN_TYPE_NAME_BITSTRING;
		case ASN_TAGNUM_OBJECT_ID:
			return ASN_TYPE_NAME_OBJECT_ID;
		case ASN_TAGNUM_OBJECT_DESCR:
			return ASN_TYPE_NAME_OBJECT_DESC;
		case ASN_TAGNUM_INT:
			return ASN_TYPE_NAME_INT;
		case ASN_TAGNUM_NULL:
			return ASN_TYPE_NAME_NULL;
		case ASN_TAGNUM_BOOLEAN:
			return ASN_TYPE_NAME_BOOL;
		default:
			return ASN_TYPE_NAME_UNKNOWN;
	}
}
