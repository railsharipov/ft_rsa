#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

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
