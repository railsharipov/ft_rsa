#include <bnum.h>

void	divmod_num_d(const t_num *a, uint64_t b, t_num *c, uint64_t *d)
{
	t_num		q;
	t_uint128	val;
	uint64_t	digit;
	int			idx;

	if (b == 0)
		BNUM_ERROR_EXIT("division by zero");

	if (b == 1 || BNUM_ZERO(a))
	{
		if (NULL != c)
			copy_num(a, c, 0, a->len);
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
			copy_num(a, c, 0, a->len);
			rsh_num_b_inpl(c, idx-1u);
		}
		if (NULL != d)
			*d = a->val[0] & ((1ull << (idx-1u)) - 1u);
		return ;
	}

	init_num_with_size(&q, a->len);
	val = 0u;

	for (idx = BNUM_MAX_DIG-1; idx > a->len-1; idx--)
		q.val[idx] = 0;

	for (; idx >= 0; idx--)
	{
		val = (val << BNUM_DIGIT_BIT) | ((t_uint128)a->val[idx]);

		if (val >= b)
		{
			digit = val / (t_uint128)b;
			val %= (t_uint128)b;
		}
		else
			digit = 0u;

		q.val[idx] = digit;
	}

	if (NULL != c)
	{
		q.len = a->len;
		q.sign = a->sign;
		skip_zeros(&q);
		copy_num(&q, c, 0, q.len);
	}
	if (NULL != d)
		*d = val & BNUM_MAX_VAL;

	clear_num(&q);
}
