#include <bnum.h>

/* Convert bignum to bytes, convert bytes to 2's complement if negative */

void	bnum_to_bytes(const t_num *num, uint8_t **bin, size_t *binsize)
{
	if (NULL == num || NULL == bin || NULL == binsize) {
		return ;
	}
	if (BNUM_ZERO(num)) {
		BNUM_ALLOC(*bin, 1);
		*bin = 0;
		*binsize = 1;
		return;
	}
	t_num *tmp = bnum_clone(num);
	size_t nbits = bnum_lmbit(tmp);
	size_t nbytes = NBITS_TO_NBYTES(nbits);

	if (tmp->sign == BNUM_NEG) {
		// Convert value to 2's complement.
		tmp->sign = BNUM_POS;
		bnum_increase_size(tmp, tmp->len+1);
		bnum_inv_bits(tmp, tmp);
		bnum_add_dig_u(tmp, 1u, tmp);
	}
	*binsize = nbytes;
	BNUM_ALLOC(*bin, *binsize);

	uint8_t *bptr = *bin + *binsize-1;
	size_t idx = 0;
	while (idx++ < *binsize) {
		*bptr-- = *(tmp->val) & 0xFF;
		bnum_rsh_bit_inpl(tmp, 8);
	}
	bnum_del(tmp);
}
