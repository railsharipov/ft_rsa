#include <bnum.h>

/* Convert bytes representing 2's complement signed integer to bignum */

void	bnum_init_from_bytes(t_num *num, const uint8_t *buf, int bufsize)
{
	if (bufsize == 0) {
		bnum_set_dig_u(num, 0u);
		return;
	}
	// This init routine ensures the result msbyte is non-zero.
	bnum_init_from_bytes_u(num, buf, bufsize);

	bool is_neg = (buf[0] & 0x80) > 0;
	if (is_neg) {
		// Find msbit position in msbyte, counting from 1.
		int nbits = bnum_lmbit(num);
		if (nbits % BNUM_DIGIT_BIT > 0) {
			// We must ensure all bits higer than current msbit are set.
			num->val[num->len-1] |= ~(((uint64_t)1u << nbits)-1u);
		}
		// 2's complement = (~n)+1
		bnum_inv_bits(num, num);
		bnum_add_dig_u(num, 1, num);
		num->sign = BNUM_NEG;
	}
}

t_num	*bnum_from_bytes(const uint8_t *buf, int bufsize)
{
	t_num *num = bnum_create();
	bnum_init(num);
	bnum_init_from_bytes(num, buf, bufsize);
	return (num);
}
