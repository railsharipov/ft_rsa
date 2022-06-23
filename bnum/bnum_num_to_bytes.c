#include <bnum.h>

void	num_to_bytes(const t_num *num, char **bin, size_t *binsize)
{
	size_t	nbits;
	char	*bptr;
	t_num	copy;
	size_t	idx;

	nbits = lmbit_num(num);
	*binsize = NBITS_TO_NBYTES(nbits);
	LIBFT_ALLOC(*bin, *binsize);

	bptr = *bin + *binsize-1;
	init_num(&copy);
	copy_num(num, &copy);

	idx = 0;
	while (idx++ < *binsize)
	{
		*bptr-- = *(copy.val) & 0xFF;
		rsh_num_b_inpl(&copy, 8);
	}

	clear_num(&copy);
}
