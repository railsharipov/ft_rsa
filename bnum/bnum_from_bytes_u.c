#include <bnum.h>

/* Convert bytes representing unsigned integer to bignum */

void	bnum_from_bytes_u(t_num *num, const uint8_t *buf, int bufsize)
{
	size_t	ndigits;
	size_t	nbits;
	int		idx;

	if (NULL == num || NULL == buf) {
		return ;
	}
	bnum_set_dig_u(num, 0u);

	if (bufsize == 0) {
		return ;
	}

	nbits = bufsize * CHAR_BIT;
	ndigits = NBITS_TO_NWORDS(nbits, BNUM_DIGIT_BIT);

	if (ndigits > num->size) {
		bnum_increase_size(num, ndigits);
	}

	idx = 0;
	while (idx < bufsize) {
		bnum_lsh_bit_inpl(num, 8);
		*(num->val) |= buf[idx++];
	}
	bnum_skip_zeros(num);
}
