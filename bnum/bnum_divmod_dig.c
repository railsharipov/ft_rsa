#include <bnum.h>

void	bnum_divmod_dig(const t_num *a, uint64_t b, t_num *c, uint64_t *d)
{
	t_num		q;
	uint128_t	val;
	uint64_t	digit;
	int			idx;

	if (b == 0)
		BNUM_ERROR_EXIT("division by zero");

	if (b == 1 || BNUM_ZERO(a))
	{
		if (NULL != c)
			bnum_copy(a, c);
		if (NULL != d)
			*d = 0;
		return ;
	}

	/* if b is power of 2 */
	if (b != 0u && (b & (b - 1u)) == 0u)
	{
		for (idx = 0; b != 0; idx++)
			b >>= 1;

		if (NULL != c)
		{
			bnum_copy(a, c);
			bnum_rsh_bit_inpl(c, idx-1u);
		}
		if (NULL != d)
			*d = a->val[0] & ((1ull << (idx-1u)) - 1u);
		return ;
	}

	bnum_init_with_size(&q, a->len);

	if (b > BNUM_MAX_VAL)
	{
		t_num	x, r;

		bnum_init(&x);
		bnum_init(&r);
		bnum_set_dig_u(&x, b);

		bnum_divmod(a, &x, &q, &r);

		if (r.len == 2)
			*d = (r.val[1] << BNUM_DIGIT_BIT) | (r.val[0] & BNUM_MAX_VAL);
		else
			*d = r.val[0] & BNUM_MAX_VAL;

		bnum_copy(&q, c);
		bnum_clear_multi(&x, &r, &q, NULL);

		return ;
	}

	val = 0u;

	for (idx = BNUM_MAX_DIG-1; idx > a->len-1; idx--)
		q.val[idx] = 0;

	for (; idx >= 0; idx--)
	{
		val = (val << BNUM_DIGIT_BIT) | ((uint128_t)a->val[idx]);

		if (val >= b)
		{
			digit = val / (uint128_t)b;
			val %= (uint128_t)b;
		}
		else
			digit = 0u;

		q.val[idx] = digit;
	}

	if (NULL != c)
	{
		q.len = a->len;
		q.sign = a->sign;
		bnum_skip_zeros(&q);
		bnum_copy(&q, c);
	}
	if (NULL != d)
		*d = val & BNUM_MAX_VAL;

	bnum_clear(&q);
}
