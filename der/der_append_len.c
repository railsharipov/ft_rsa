#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static int	__is_long_form(size_t len);
static void	__append_long_form(t_der *der, size_t len);
static void	__append_short_form(t_der *der, size_t len);

int  der_append_len(t_der *der, size_t len)
{
	if (NULL == der)
		return (SSL_ERROR(INVALID_INPUT));

	if (__is_long_form(len))
		__append_long_form(der, len);
	else
		__append_short_form(der, len);

	return (SSL_OK);
}

static int	__is_long_form(size_t len)
{
	return (len >= 0x7F);
}

static void	__append_long_form(t_der *der, size_t len)
{
	char	*len_buf;
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

	der_append_content(der, len_buf, len_buf_size);
	SSL_FREE(len_buf);
}

static void	__append_short_form(t_der *der, size_t len)
{
	const size_t	len_buf_size = 1;
	char			len_buf[len_buf_size];

	len_buf[0] = ASN_LEN_SHORT;
	len_buf[0] |= len;

	der_append_content(der, len_buf, len_buf_size);
}
