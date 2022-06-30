#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <bnum.h>

static const int	ASN_TAG = (ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE);
static const int	ASN_PRIMITIVE = (ASN_ENCODE_PRIMITIVE | ASN_TAG);

int  der_read_bool(t_iasn *item, char **derenc, size_t *dersize)
{
	unsigned char	*octets;
	size_t			osize;
	size_t			olen;

	SSL_CHECK(NULL != item);
	SSL_CHECK((NULL != derenc) && (NULL != *derenc));
	SSL_CHECK(NULL != dersize);

	octets = (unsigned char *)(*derenc);
	osize = *dersize;

	if ((ASN_PRIMITIVE | ASN_TAG_BOOLEAN) != *octets)
		return (DER_ERROR(INVALID_ASN_TYPE_TAG));

	octets++;
	osize--;

	if (SSL_OK != der_read_len(&octets, &osize, &olen))
		return (DER_ERROR(INVALID_ASN_LEN_TAG));

	if (olen > osize)
		return (DER_ERROR(INVALID_ASN_LEN_TAG));

	SSL_ALLOC(item->content, olen);
	ft_memcpy(item->content, octets, olen);
	item->bitsize = NBYTES_TO_NBITS(olen);

	*derenc = (char *)(octets) + olen;
	*dersize = osize - olen;

	return (SSL_OK);
}

int  der_read_bool_octets(t_iasn *item, unsigned char *derenc, size_t dersize)
{
	const size_t	bool_size = 1;
	unsigned char	bool_value;

	if (NULL == item || NULL == derenc)
		return (DER_ERROR(INVALID_INPUT));

	bool_value = 0;

	while (dersize-- != 0 && bool_value == 0)
		if (*derenc++ != 0)
			bool_value = 1;

	SSL_ALLOC(item->content, bool_size);
	ft_memcpy(item->content, &bool_value, bool_size);
	item->bitsize = NBYTES_TO_NBITS(bool_size);

	return (SSL_OK);
}
