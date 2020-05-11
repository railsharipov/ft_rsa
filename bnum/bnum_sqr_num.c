#include <bnum.h>

void	sqr_num(const t_num *a, t_num *res)
{
	if (a->len < BNUM_KAR_THRES)
		sqr_num_comba(a, res);
	else
		sqr_num_karatsuba(a, res);
}
