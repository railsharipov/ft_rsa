#include <util/bnum.h>

/* Convert bignum to bytes, sign is ignored */

void	bnum_to_bytes_u(const t_num *num, char **bin, size_t *binsize)
{
	size_t	nbits;
	char	*bptr;
	t_num	copy;
	size_t	idx;

	if (NULL == num || NULL == bin || NULL == binsize)
		return ;

	nbits = bnum_lmbit(num);
	*binsize = NBITS_TO_NBYTES(nbits);
	BNUM_ALLOC(*bin, *binsize);

	bptr = *bin + *binsize-1;
	bnum_init(&copy);
	bnum_copy(num, &copy);

	idx = 0;
	while (idx++ < *binsize)
	{
		*bptr-- = *(copy.val) & 0xFF;
		bnum_rsh_bit_inpl(&copy, 8);
	}

	bnum_clear(&copy);
}
