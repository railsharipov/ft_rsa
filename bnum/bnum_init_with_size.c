#include <bnum.h>

void	bnum_init_with_size(t_num *num, int size)
{
	size = (size < BNUM_MAX_DIG) ? (BNUM_MAX_DIG) : (size);
	LIBFT_ALLOC(num->val, size * sizeof(uint64_t));
	num->size = size;
	num->len = 1;
	num->sign = BNUM_POS;
}
