#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

int der_append_null(t_der *der, void *content, size_t cont_nbits)
{
	if (NULL == der)
		return (DER_ERROR(INVALID_INPUT));

	(void)content;
	(void)cont_nbits;

	der_append_id_tag(
		der,
		ASN_ENCODE_PRIMITIVE | ASN_TAG_UNIVERSAL,
		ASN_TAG_NULL);

	der_append_len_new(der, 0);

	return (SSL_OK);
}
