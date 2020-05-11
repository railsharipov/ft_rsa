#include <bnum.h>

void	add_num_d(const t_num *a, uint64_t b, t_num *res)
{
	if (BNUM_POS == a->sign)
	{
		add_num_ud(a, b, res);
		res->sign = BNUM_POS;
	}
	else
	{
		if (a->len > 1)
		{
			sub_num_ud(a, b, res);
			res->sign = BNUM_NEG;
		}
		else
		{
			init_num(res);
			if (a->val[0] > b)
			{
				res->val[0] = a->val[0] - b;
				res->sign = BNUM_NEG;
			}
			else if (a->val[0] < b)
			{
				res->val[0] = b - a->val[0];
				res->sign = BNUM_POS;
			}
		}
	}
}
