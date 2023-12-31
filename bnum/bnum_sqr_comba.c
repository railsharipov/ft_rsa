#include <bnum.h>

void	bnum_sqr_comba(const t_num *a, t_num *res)
{
	uint64_t	*temp;
	t_uint128	val, carry;
	int 		idx, ndig, z;

	carry = 0;
	ndig = 2 * a->len;

	if (ndig > res->size)
		bnum_increase_size(res, ndig);

	LIBFT_ALLOC(temp, sizeof(uint64_t) * BNUM_MAX_DIG);

	for (int idx = 0; idx < ndig; idx++)
	{
		const uint64_t	*aptr, *bptr;
		int				n, y, x;

		y = BNUM_MIN(idx, a->len-1);
		x = idx - y;
		n = BNUM_MIN(a->len-x, y+1);
		n = BNUM_MIN(n, (y-x+1)>>1);

		aptr = a->val + x;
		bptr = a->val + y;

		val = 0;
		for (z = 0; z < n; z++)
			val += (t_uint128) *aptr++ * *bptr--;

		val = (val<<1) + carry;
		if (!(idx & 0x1))
			val += (t_uint128) a->val[idx>>1] * a->val[idx>>1];

		temp[idx] = val & BNUM_MAX_VAL;
		carry = val >> BNUM_DIGIT_BIT;
	}

	for (int i = 0; i < ndig; i++)
		res->val[i] = temp[i];
	for (int i = ndig; i < res->size; i++)
		res->val[i] = 0;

	res->len = ndig;
	res->sign = BNUM_POS;
	bnum_skip_zeros(res);

	LIBFT_FREE(temp);
}
