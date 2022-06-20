#include <bnum.h>

void	mul_num_comba_hi(const t_num *a, const t_num *b, t_num *res, int dig)
{
	uint64_t	*temp;
	t_uint128	val;
	int 		ndig, idx, z;

	val = 0;
	ndig = a->len + b->len;

	if (ndig > res->size)
		increase_num_size(res, ndig);

	LIBFT_ALLOC(temp, sizeof(uint64_t) * ndig);

	dig = BNUM_MAX(0, dig-2);

	for (int idx = dig; idx < ndig; idx++)
	{
		const uint64_t	*aptr, *bptr;
		int				n, y, x;

		y = BNUM_MIN(idx, b->len-1);
		x = idx - y;
		n = BNUM_MIN(a->len-x, y+1);

		aptr = a->val + x;
		bptr = b->val + y;

		for (z = 0; z < n; z++)
			val += (t_uint128) *aptr++ * *bptr--;

		temp[idx] = val & BNUM_MAX_VAL;
		val >>= BNUM_DIGIT_BIT;
	}

	for (int i = dig; i < ndig; i++)
		res->val[i] = temp[i];
	for (int i = ndig; i < res->size; i++)
		res->val[i] = 0;

	res->len = ndig;
	res->sign = a->sign * b->sign;
	skip_zeros(res);

	LIBFT_FREE(temp);
}
