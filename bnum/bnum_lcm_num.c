#include <bnum.h>

void	lcm_num(const t_num *a, const t_num *b, t_num *res)
{
	t_num	gcd;

	gcd_num(a, b, &gcd);
	mul_num(a, b, res);

	if (gcd.len == 1)
		divmod_num_d(res, gcd.val[0], res, NULL);
	else
		divmod_num(res, &gcd, res, NULL);
}
