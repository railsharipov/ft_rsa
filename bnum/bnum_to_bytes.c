#include <bnum.h>

/* Convert bignum to bytes, convert bytes to 2's complement if negative */

void	bnum_to_bytes(const t_num *num, char **bin, size_t *binsize)
{
	if (NULL == num || NULL == bin || NULL == binsize) {
		return ;
	}
	t_num *tmp = bnum_clone(num);

	if (tmp->sign == BNUM_NEG) {
		// Convert value to 2's complement.
		tmp->sign = BNUM_POS;
		bnum_inv_bits(tmp, tmp);
		bnum_add_dig_u(tmp, 1u, tmp);
	}
	size_t nbits = bnum_lmbit(tmp);
	*binsize = NBITS_TO_NBYTES(nbits);
	BNUM_ALLOC(*bin, *binsize);

	char *bptr = *bin + *binsize-1;
	size_t idx = 0;
	while (idx++ < *binsize) {
		*bptr-- = *(tmp->val) & 0xFF;
		bnum_rsh_bit_inpl(tmp, 8);
	}
	bnum_del(tmp);
}
