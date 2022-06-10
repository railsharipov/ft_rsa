#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <bnum.h>

int der_append_int(t_der *der, void *content, size_t nbits)
{
	int		id_nbytes, len_nbytes, enc_nbytes, cont_nbytes;
	int		sign_nbytes;
	char	*precontent;
	t_num	*num;

	if (NULL == der)
		return (DER_ERROR(INVALID_INPUT));

	id_nbytes = 1; // since simple tag expected

	num = (t_num *)(content);
	content = stringify_num(num);
	nbits = lmbit_num(num);
	cont_nbytes = TO_NUM_BYTES(nbits);

	// Special case for zero
	if (cont_nbytes == 0)
		cont_nbytes = 1;

	// In case len tag is long (len bytes > 127), extra 1 byte added
	SSL_ALLOC(precontent, (1+sizeof(cont_nbytes))+id_nbytes);

	*precontent = ASN_ENCODE_PRIMITIVE | ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE;
	*precontent |= ASN_TAG_INT;

	// Since current big number implementation does not use MSB
	// to store integer sign ('sign' boolean is used instead),
	// one more byte is required in case MSB of encoded big number
	// is set to one
	if (((unsigned char *)content)[0] & (1u<<7))
		sign_nbytes = 1;
	else
		sign_nbytes = 0;

	len_nbytes = der_append_len(precontent + id_nbytes, cont_nbytes+sign_nbytes);
	enc_nbytes = id_nbytes + len_nbytes + cont_nbytes + sign_nbytes;

	SSL_REALLOC(der->content, der->size, der->size + enc_nbytes);
	ft_memcpy(der->content + der->size, precontent, id_nbytes + len_nbytes);
	der->size += id_nbytes + len_nbytes;

	if (sign_nbytes != 0)
	{
		((unsigned char *)(der->content))[der->size] = (1u<<7)*(int)(BNUM_NEG==num->sign);
		der->size += 1;
	}
	ft_memcpy(der->content + der->size, content, cont_nbytes);
	der->size += cont_nbytes;

	SSL_FREE(precontent);

	return (SSL_OK);
}
