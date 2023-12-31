#include <bnum.h>

void	add_dig(const t_num *a, uint64_t b, t_num *res)
{
	t_num	tmp;

	init_num_with_size(&tmp, 2);

	tmp.val[0] = b & BNUM_MAX_VAL;
	tmp.val[1] = b >> BNUM_DIGIT_BIT;
	tmp.len = 2;

	skip_zeros(&tmp);
	add_num(a, &tmp, res);
	clear_num(&tmp);
}
