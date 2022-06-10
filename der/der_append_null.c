#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

int der_append_null(t_der *der, void *content, size_t nbits)
{
	int	idx;
	int	id_nbytes, len_nbytes, enc_nbytes;
	char	*precontent;

	if (NULL == der)
		return (DER_ERROR(INVALID_INPUT));

	(void)content;
	(void)nbits;
	id_nbytes = 1; // since simple tag expected

	SSL_ALLOC(precontent, id_nbytes+1);

	*precontent = ASN_ENCODE_PRIMITIVE | ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE;
	*precontent |= ASN_TAG_NULL;

	len_nbytes = der_append_len(precontent + id_nbytes, 0);
	enc_nbytes = id_nbytes + len_nbytes;

	SSL_REALLOC(der->content, der->size, der->size + enc_nbytes);
	ft_memcpy(der->content + der->size, precontent, enc_nbytes);
	der->size += enc_nbytes;

	SSL_FREE(precontent);

	return (SSL_OK);
}
