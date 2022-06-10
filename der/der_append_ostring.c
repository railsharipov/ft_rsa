#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

int der_append_ostring(t_der *der, void *content, size_t nbits)
{
	int		id_nbytes, len_nbytes, enc_nbytes, cont_nbytes;
	char	*precontent;

	if (NULL == der)
		return (DER_ERROR(INVALID_INPUT));

	id_nbytes = 1; // since simple tag expected
	cont_nbytes = TO_NUM_BYTES(nbits);

	SSL_ALLOC(precontent, (1+sizeof(cont_nbytes))+id_nbytes);

	*precontent = ASN_ENCODE_PRIMITIVE | ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE;
	*precontent |= ASN_TAG_OCTET_STRING;

	len_nbytes = der_append_len(precontent + id_nbytes, cont_nbytes);
	enc_nbytes = id_nbytes + len_nbytes + cont_nbytes;

	SSL_REALLOC(der->content, der->size, der->size + enc_nbytes);
	ft_memcpy(der->content + der->size, precontent, id_nbytes + len_nbytes);
	der->size += id_nbytes + len_nbytes;
	ft_memcpy(der->content + der->size, content, cont_nbytes);
	der->size += cont_nbytes;

	SSL_FREE(precontent);

	return (SSL_OK);
}
