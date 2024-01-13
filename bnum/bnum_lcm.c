#include <util/bnum.h>

void	bnum_lcm(const t_num *a, const t_num *b, t_num *res)
{
	t_num	gcd;

	bnum_init(&gcd);

	bnum_gcd(a, b, &gcd);
	bnum_mul(a, b, res);

	if (gcd.len == 1)
		bnum_divmod_dig(res, gcd.val[0], res, NULL);
	else
		bnum_divmod(res, &gcd, res, NULL);

	bnum_clear(&gcd);
}
