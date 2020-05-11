#include <bnum.h>

void	sqr_num_comba(const t_num *a, t_num *res)
{
#ifdef BNUM_HEAP_WINDOW
	uint64_t	*temp;
#else
	uint64_t	temp[BNUM_MAX_DIG];
#endif
	t_uint128	value, carry;
	int 		idx, ndig, z;

	carry = 0;
	ndig = 2 * a->len;

	if (ndig > BNUM_MAX_DIG)
		BNUM_ERROR("big number size limit exceeded");

#ifdef BNUM_HEAP_WINDOW
	LIBFT_ALLOC(temp, sizeof(uint64_t) * BNUM_MAX_DIG);
#endif

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

		value = 0;
		for (z = 0; z < n; z++)
			value += (t_uint128) *aptr++ * *bptr--;

		value = (value<<1) + carry;
		if (!(idx & 0x1))
			value += (t_uint128) a->val[idx>>1] * a->val[idx>>1];

		temp[idx] = value & BNUM_MAX_VAL;
		carry = value >> BNUM_DIGIT_BIT;
	}

	for (int i = 0; i < ndig; i++)
		res->val[i] = temp[i];
	for (int i = ndig; i < BNUM_MAX_DIG; i++)
		res->val[i] = 0;

	res->len = ndig;
	res->sign = BNUM_POS;
	skip_zeros(res);

#ifdef BNUM_HEAP_WINDOW
	LIBFT_FREE(temp);
#endif
}
