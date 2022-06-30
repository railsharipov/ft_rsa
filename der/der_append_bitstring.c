#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

int der_append_bitstring(t_der *der, void *content, size_t cont_nbits)
{
	const int		num_octets_for_num_unused_bits = 1;
	unsigned char	num_unused_bits;
	size_t			cont_nbytes;

	if (NULL == der)
		return (DER_ERROR(INVALID_INPUT));

	// number of unused bits in the last content octet
	num_unused_bits = 8 - cont_nbits % 8;
	cont_nbytes = NBITS_TO_NBYTES(cont_nbits);

	der_append_id_tag(
		der,
		ASN_ENCODE_PRIMITIVE | ASN_TAG_UNIVERSAL,
		ASN_TAG_BIT_STRING);

	der_append_len(der, cont_nbytes + num_octets_for_num_unused_bits);
	der_append_content(der, &num_unused_bits, num_octets_for_num_unused_bits);
	der_append_content(der, content, cont_nbytes);

	return (SSL_OK);
}
