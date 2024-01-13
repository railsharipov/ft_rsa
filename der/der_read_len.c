#include <ssl/ssl.h>
#include <ssl/error.h>
#include <util/io.h>
#include <ssl/asn.h>
#include <ssl/der.h>

static int	__len_is_long(char);

ssize_t	der_read_len(size_t *len, uint8_t *form, t_iodes *iodes)
{
	char		octet;
	ssize_t		rsize;
	ssize_t		tsize;
	size_t		lensize;

	if (NULL == len || NULL == form || NULL == iodes)
		return (-1);

	*len = 0;

	if ((tsize = der_read_octets(&octet, 1, iodes)) <= 0)
		return (-1);

	if (__len_is_long(octet))
	{
		*form = ASN_LEN_LONG;
		lensize = octet & 0x7F;

		while (lensize > 0)
		{
			if ((rsize = der_read_octets(&octet, 1, iodes)) <= 0)
				break ;

			*len <<= CHAR_BIT;
			*len |= (size_t)octet;

			tsize += rsize;
			lensize--;
		}

		if (lensize > 0)
			return (-1);
	}
	else
	{
		*form = ASN_LEN_SHORT;
		*len = (size_t)octet;
	}

	return (tsize);
}

static int	__len_is_long(char len)
{
	return (len & ASN_LEN_LONG);
}
