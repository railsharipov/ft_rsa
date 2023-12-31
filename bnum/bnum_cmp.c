#include <bnum.h>

int bnum_cmp(const t_num *a, const t_num *b)
{
	if (a->sign != b->sign)
	{
		if (a->sign > b->sign)
			return (1);
		else
			return (-1);
	}
	
	if (BNUM_POS == a->sign)
		return (bnum_cmp_u(a, b));
	else
		return (bnum_cmp_u(b, a));
}
