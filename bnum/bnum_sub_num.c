#include <bnum.h>

void	sub_num(const t_num *a, const t_num *b, t_num *res)
{
	int	asign;
	int bsign;

	asign = BNUM_SIGN(a);
	bsign = BNUM_SIGN(b);
	if (asign != bsign)
	{
		add_num_u(a, b, res);
		res->sign = asign;
	}
	else
	{
		if (compare_num_u(a, b) >= 0)
		{
			sub_num_u(a, b, res);
			res->sign = asign;
		}
		else
		{
			sub_num_u(b, a, res);
			res->sign = (BNUM_POS == asign) ? (BNUM_NEG):(BNUM_POS);
		}
	}
}
