#include <util/bnum.h>

int     bnum_cmp_dig(const t_num *a, uint64_t b)
{
	t_num	num;
	int		res;

	if (BNUM_NEG == a->sign)
		return (-1);

	if (a->len > 2)
		return (1);

	if ((b <= BNUM_MAX_VAL) && (a->len < 2))
	{
		if (a->val[0] < b)
			return (-1);
		else
			return (a->val[0] - b);
	}

	bnum_init(&num);
	bnum_set_dig_u(&num, b);

	res = bnum_cmp_u(a, &num);

	bnum_clear(&num);

	return (res);
}
