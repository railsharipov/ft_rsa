#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <bnum.h>

static const int	ASN_TAG = (ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE);
static const int	ASN_PRIMITIVE = (ASN_ENCODE_PRIMITIVE | ASN_TAG);

int  der_read_int(t_iasn *item, char **derenc, size_t *dersize)
{
	unsigned char	*octets;
	size_t			osize;
	size_t			olen;
	t_num			*num;

	SSL_CHECK(NULL != item);
	SSL_CHECK((NULL != derenc) && (NULL != *derenc));
	SSL_CHECK(NULL != dersize);

	octets = (unsigned char *)(*derenc);
	osize = *dersize;
	num = item->content;

	if ((ASN_PRIMITIVE | ASN_TAG_INT) != *octets)
		return (DER_ERROR(INVALID_ASN_TYPE_TAG));

	octets++;
	osize--;

	if (SSL_OK != der_read_len(&octets, &osize, &olen))
		return (DER_ERROR(INVALID_ASN_LEN_TAG));

	if (olen > osize)
		return (DER_ERROR(INVALID_ASN_LEN_TAG));

	if (olen * CHAR_BIT > BNUM_MAX_DIG * BNUM_DIGIT_BIT)
		return (DER_ERROR(UNSPECIFIED_ERROR));

	bytes_to_num(num, (char *)octets, olen);

	*derenc = (char *)(octets) + olen;
	*dersize = osize - olen;

	return (SSL_OK);
}

int  der_read_int_octets(t_iasn *item, char *derenc, size_t dersize)
{
	return (der_read_ostring_octets(item, derenc, dersize));
}
