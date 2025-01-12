#include <bnum.h>

void	bnum_init(t_num *num)
{
	BNUM_ALLOC(num->val, BNUM_MIN_DIG_COUNT * sizeof(uint64_t));
	num->size = BNUM_MIN_DIG_COUNT;
	num->len = 1;
	num->sign = BNUM_POS;
}
