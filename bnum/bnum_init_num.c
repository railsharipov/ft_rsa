#include <bnum.h>

void	init_num(t_num *num)
{
	LIBFT_ALLOC(num->val, BNUM_MAX_DIG * sizeof(uint64_t));
	num->size = BNUM_MAX_DIG;
	num->len = 1;
	num->sign = BNUM_POS;
}
