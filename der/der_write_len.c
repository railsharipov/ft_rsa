#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <ssl_io.h>

static int		__is_long_form(size_t len);
static ssize_t	__write_long_form(size_t len, t_iodes *iodes);
static ssize_t	__write_short_form(size_t len, t_iodes *iodes);

ssize_t  der_write_len(size_t len, t_iodes *iodes)
{
	ssize_t	wbytes;

	if (NULL == iodes)
		return (-1);

	if (__is_long_form(len))
		wbytes = __write_long_form(len, iodes);
	else
		wbytes = __write_short_form(len, iodes);

	return (wbytes);
}

static int	__is_long_form(size_t len)
{
	return (len >= 0x7F);
}

static ssize_t	__write_long_form(size_t len, t_iodes *iodes)
{
	ssize_t	wbytes;
	uint8_t	*len_buf;
	int		len_buf_size;
	int		len_nbytes;
	int		len_nbits;
	int		idx;

	len_nbits = util_lmbit(len, 8 * sizeof(len));
	len_nbytes = NBITS_TO_NBYTES(len_nbits);

	// additional octet for long form flag
	len_buf_size = 1 + len_nbytes;
	SSL_ALLOC(len_buf, len_buf_size);

	idx = 0;
	len_buf[idx] = ASN_LEN_LONG;
	len_buf[idx++] |= len_nbytes;

	len_nbits = CEIL(len_nbits, 8);

	while (len_nbits > 0)
	{
		len_buf[idx++] = len >> (len_nbits-8);
		len_nbits -= 8;
	}

	wbytes = der_write_octets((char *)len_buf, len_buf_size, iodes);

	SSL_FREE(len_buf);

	return (wbytes);
}

static ssize_t	__write_short_form(size_t len, t_iodes *iodes)
{
	const size_t	len_buf_size = 1;
	uint8_t			len_buf[len_buf_size];

	len_buf[0] = ASN_LEN_SHORT;
	len_buf[0] |= len;

	return (der_write_octets((char *)len_buf, len_buf_size, iodes));
}
