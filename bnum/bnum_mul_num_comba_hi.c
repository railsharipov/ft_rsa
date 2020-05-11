#include <bnum.h>

void	mul_num_comba_hi(const t_num *a, const t_num *b, t_num *res, int dig)
{
#ifdef BNUM_HEAP_WINDOW
	uint64_t	*temp;
#else
	uint64_t	temp[BNUM_MAX_DIG];
#endif
	t_uint128	value;
	int 		plen, idx, z;

	value = 0;
	plen = a->len + b->len;

	if (plen > BNUM_MAX_DIG)
		BNUM_ERROR("big number size limit exceeded");

#ifdef BNUM_HEAP_WINDOW
	LIBFT_ALLOC(TEMP, sizeof(uint64_t) * BNUM_MAX_DIG);
#endif

	dig = BNUM_MAX(0, dig-2);

	for (int idx = dig; idx < plen; idx++)
	{
		const uint64_t	*aptr, *bptr;
		int				n, y, x;

		y = BNUM_MIN(idx, b->len-1);
		x = idx - y;
		n = BNUM_MIN(a->len-x, y+1);

		aptr = a->val + x;
		bptr = b->val + y;

		for (z = 0; z < n; z++)
			value += (t_uint128) *aptr++ * *bptr--;

		temp[idx] = value & BNUM_MAX_VAL;
		value >>= BNUM_DIGIT_BIT;
	}

	for (int i = dig; i < plen; i++)
		res->val[i] = temp[i];
	for (int i = plen; i < BNUM_MAX_DIG; i++)
		res->val[i] = 0;

	res->len = plen;
	res->sign = a->sign * b->sign;
	skip_zeros(res);

#ifdef BNUM_HEAP_WINDOW
	LIBFT_FREE(temp);
#endif
}
