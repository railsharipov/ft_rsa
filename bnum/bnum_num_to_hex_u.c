#include <bnum.h>

static const char	A[] = "0123456789abcdef";

/* Convert bnum value to hexstring, sign is ignored */

char	*num_to_hex_u(t_num *num)
{
	char		*hexrev, *hptr, *hex, *hexresult;
	uint64_t	digit;
	size_t		hexsize;
	int			idx, idy, offset;

	hexsize = NBITS_TO_NWORDS(num->len * BNUM_DIGIT_BIT, 4);
	LIBFT_ALLOC(hexrev, hexsize+1);

	hptr = hexrev;
	idx = 0;

	while (idx < num->len)
	{
		digit = num->val[idx];

		idy = 0;
		while (idy < BNUM_DIGIT_BIT)
		{
			*hptr++ = A[(digit >> idy) & 0xF];
			idy += 4;
		}
		idx++;
	}

	hexrev[hexsize] = 0;
	hex = ft_strrev(hexrev);

	offset = 0;
	while (hex[offset] == '0' && offset < hexsize-1)
		offset++;

	hexresult = ft_strdup(hex + offset);
	LIBFT_FREE(hexrev);
	LIBFT_FREE(hex);

	return (hexresult);
}
