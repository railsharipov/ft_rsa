#include <bnum.h>

void	bnum_add(const t_num *a, const t_num *b, t_num *res)
{
	int	asign;
	int	bsign;

	asign = BNUM_SIGN(a);
	bsign = BNUM_SIGN(b);

	if (asign == bsign)
	{
		bnum_add_u(a, b, res);
		res->sign = asign;
	}
	else
	{
		if (bnum_cmp_u(a, b) >= 0)
		{
			bnum_sub_u(a, b, res);
			res->sign = asign;
		}
		else
		{
			bnum_sub_u(b, a, res);
			res->sign = bsign;
		}
	}
}
