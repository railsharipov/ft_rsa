#include <bnum.h>
#include <libft/string.h>

static const char	A[] = "0123456789abcdef";

/* Convert bnum value to hex number string, sign is ignored */

char	*bnum_to_hex_u(const t_num *num)
{
	char		*hexrev, *hptr, *hex, *hexresult;
	size_t		hexsize;
	int			idx, offset;
	t_num		copy;

	if (NULL == num) {
		return (NULL);
	}
	if (BNUM_ZERO(num)) {
		return (strdup("0"));
	}
	hexsize = NBITS_TO_NWORDS(num->len * BNUM_DIGIT_BIT, 4);
	BNUM_ALLOC(hexrev, hexsize+1);

	hptr = hexrev;

	bnum_init(&copy);
	bnum_copy(num, &copy);

	while (!(BNUM_ZERO(&copy))) {
		*hptr++ = A[copy.val[0] & 0xF];
		bnum_rsh_bit_inpl(&copy, 4);
	}
	hexrev[hexsize] = 0;
	hex = ft_strrev(hexrev);

	offset = 0;
	while (hex[offset] == '0' && offset < hexsize-1) {
		offset++;
	}
	hexresult = ft_strdup(hex + offset);
	BNUM_FREE(hexrev);
	BNUM_FREE(hex);

	return (hexresult);
}
