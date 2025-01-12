#include <bnum.h>

void	bnum_sqr(const t_num *a, t_num *res)
{
	if (a->len < BNUM_KAR_THRES)
		bnum_sqr_comba(a, res);
	else
		bnum_sqr_karatsuba(a, res);
}
