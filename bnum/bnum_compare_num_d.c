#include <bnum.h>

int     compare_num_d(const t_num *a, uint64_t b)
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

	init_num(&num);
	set_num_d(&num, b);

	res = compare_num_u(a, &num);

	clear_num(&num);

	return (res);
}
