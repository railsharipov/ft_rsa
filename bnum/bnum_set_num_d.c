#include <bnum.h>

void    set_num_ud(t_num *num, uint64_t digit)
{
	if (num->len < 2)
	{
		increase_num_size(num, 2);
	}
	if (digit <= BNUM_MAX_VAL)
	{
		num->val[0] = digit;
		num->len = 1;
	}
	else
	{
		num->val[0] = digit & BNUM_MAX_VAL;
		num->val[1] = digit >> BNUM_DIGIT_BIT;
		num->len = 2;
	}
	num->sign = BNUM_POS;
}
