#include <bnum.h>

/* Convert bignum to bytes, sign is ignored */

void	bnum_to_bytes_u(const t_num *num, uint8_t **bin, size_t *binsize)
{
	size_t	idx;

	if (NULL == num || NULL == bin || NULL == binsize) {
		return ;
	}

	size_t nbits = bnum_lmbit(num);
	*binsize = NBITS_TO_NBYTES(nbits);
	BNUM_ALLOC(*bin, *binsize);

	uint8_t *bptr = *bin + *binsize-1;
	t_num *copy = bnum_clone(num);
	idx = 0;
	while (idx++ < *binsize) {
		*bptr-- = *(copy->val) & 0xFF;
		bnum_rsh_bit_inpl(copy, 8);
	}
	bnum_del(copy);
}
