#include <ssl/ssl.h>
#include <ssl/der.h>
#include <ssl/asn.h>
#include <util/io.h>

static int	__tag_is_complex(uint32_t);

ssize_t	der_read_tag(uint8_t *tag, uint32_t *tagnum, t_iodes *iodes)
{
	char		octet;
	ssize_t		rsize;
	ssize_t		tsize;

	if (NULL == tag || NULL == tagnum || NULL == iodes)
		return (-1);

	*tag = 0;
	tsize = 0;

	if ((tsize = der_read_octets((char *)tag, 1, iodes)) < 0)
		return (-1);

	*tagnum = *tag & 0x1F;
	*tag = *tag & 0xE0;

	if (__tag_is_complex(*tagnum))
	{
		*tagnum = 0;

		while ((rsize = der_read_octets(&octet, 1, iodes)) > 0)
		{
			*tagnum <<= 7;
			*tagnum |= (uint32_t)octet & 0x7F;

			tsize += rsize;
		}

		if ((rsize < 0) || !(octet & 0x80))
			return (-1);
	}

	return (tsize);
}

static inline int	__tag_is_complex(uint32_t tagnum)
{
	return (ASN_TAGNUM_COMPLEX == tagnum);
}
