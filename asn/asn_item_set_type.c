#include <common.h>
#include <asn.h>
#include <bnum.h>
#include <libft/htable.h>

static inline int	__eq(const char *a, const char *b) {
	return (0 == ft_strcmp(a, b));
}

int	asn_item_set_type(t_iasn *item, char *type_key)
{
	uint8_t	tagnum;

	if (NULL == type_key || NULL == item) {
		return (ASN_LOG(ERROR, INVALID_INPUT_ERROR));
	}

	if (__eq(type_key, ASN_TYPE_KEY_SEQUENCE)) {
		tagnum = ASN_TAGNUM_SEQUENCE;
	}
	else if (__eq(type_key, ASN_TYPE_KEY_OSTRING)) {
		tagnum = ASN_TAGNUM_OCTET_STRING;
	}
	else if (__eq(type_key, ASN_TYPE_KEY_BITSTRING)) {
		tagnum = ASN_TAGNUM_BIT_STRING;
	}
	else if (__eq(type_key, ASN_TYPE_KEY_OBJECTID)) {
		tagnum = ASN_TAGNUM_OBJECT_ID;
	}
	else if (__eq(type_key, ASN_TYPE_KEY_INT)) {
		tagnum = ASN_TAGNUM_INT;
	}
	else if (__eq(type_key, ASN_TYPE_KEY_NULL)) {
		tagnum = ASN_TAGNUM_NULL;
	}
	else if (__eq(type_key, ASN_TYPE_KEY_BOOL)) {
		tagnum = ASN_TAGNUM_BOOLEAN;
	}
	else {
		return (ASN_LOG(ERROR, INVALID_INPUT_ERROR));
	}
	item->tagnum = tagnum;

	return (SSL_OK);
}
