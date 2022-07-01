#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

int der_append_ostring(t_der *der, void *content, size_t cont_nbits)
{
	size_t	cont_nbytes;

	if (NULL == der)
		return (DER_ERROR(INVALID_INPUT));

	cont_nbytes = NBITS_TO_NBYTES(cont_nbits);

	der_append_id_tag(
		der,
		ASN_ENCODE_PRIMITIVE | ASN_TAG_UNIVERSAL,
		ASN_TAG_OCTET_STRING);

	der_append_len(der, cont_nbytes);
	der_append_content(der, content, cont_nbytes);

	return (SSL_OK);
}
