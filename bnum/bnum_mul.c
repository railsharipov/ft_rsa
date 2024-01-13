#include <util/bnum.h>

void	bnum_mul(const t_num *a, const t_num *b, t_num *res)
{
	if (BNUM_MIN(a->len, b->len) < BNUM_KAR_THRES)
		bnum_mul_comba(a, b, res);
	else
		bnum_mul_karatsuba(a, b, res);
}
