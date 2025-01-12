#include <bnum.h>

void	bnum_sub_dig(const t_num *a, uint64_t b, t_num *res)
{
	t_num	tmp;

	bnum_init_with_size(&tmp, 2);

	tmp.val[0] = b & BNUM_MAX_VAL;
	tmp.val[1] = b >> BNUM_DIGIT_BIT;
	tmp.len = 2;

	bnum_skip_zeros(&tmp);
	bnum_sub(a, &tmp, res);
	bnum_clear(&tmp);
}
