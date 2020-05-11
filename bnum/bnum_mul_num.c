#include <bnum.h>

void	mul_num(const t_num *a, const t_num *b, t_num *res)
{
	if (BNUM_MIN(a->len, b->len) < BNUM_KAR_THRES)
		mul_num_comba(a, b, res);
	else
		mul_num_karatsuba(a, b, res);
}
