#include <util/bnum.h>

void    bnum_skip_zeros(t_num *num)
{
	while (num->len > 1 && num->val[num->len-1] == 0)
		num->len--;

	if (BNUM_ZERO(num))
		num->sign = BNUM_POS;
}
