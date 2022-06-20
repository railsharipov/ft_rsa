#include <bnum.h>

void	reset_num(t_num *num)
{
	int	idx;

	for (idx = 0; idx < num->size; idx++)
		num->val[idx] = 0;

	num->len = 1;
	num->sign = BNUM_POS;
}
