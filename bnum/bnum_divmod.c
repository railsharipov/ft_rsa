#include <bnum.h>

void	bnum_divmod(const t_num *a, const t_num *b, t_num *c, t_num *d)
{
	t_num	q, x, y, t1, t2;
	int		shift, n, t, i;
	int		asign, bsign;

	asign = a->sign;
	bsign = b->sign;

	if (BNUM_ZERO(b))
		BNUM_ERROR_EXIT("division by zero");

	if (bnum_cmp_u(a, b) < 0)
	{
		if (NULL != d)
			bnum_copy(a, d);
		if (NULL != c)
			bnum_set_dig_u(c, 0);
		return ;
	}

	bnum_init_multi(&t1, &t2, &x, &y, NULL);
	bnum_init_with_size(&q, a->len);

	bnum_abs(a, &x);
	bnum_abs(b, &y);

	shift = BNUM_DIGIT_BIT - bnum_lmbit(&y) % BNUM_DIGIT_BIT;
	bnum_lsh_bit_inpl(&x, shift);
	bnum_lsh_bit_inpl(&y, shift);

	n = x.len-1;
	t = y.len-1;

	bnum_lsh_dig_inpl(&y, n-t);
	while (bnum_cmp(&x, &y) >= 0)
	{
		q.val[n-t] += 1u;
		bnum_sub(&x, &y, &x);
	}

	bnum_rsh_dig_inpl(&y, n-t);

	for (i = n; i >= t+1; i--)
	{
		if (i > x.len)
			continue ;
		if (x.val[i] == y.val[t])
			q.val[i-t-1] = BNUM_MAX_VAL;
		else
		{
			uint128_t	val;

			val = ((uint128_t) x.val[i]) << BNUM_DIGIT_BIT;
			val |= (uint128_t) x.val[i-1];
			val /= (uint128_t) y.val[t];
			q.val[i-t-1] = val;
		}

		q.val[i-t-1] = (q.val[i-t-1]+1u) & BNUM_MAX_VAL;

		do
		{
			q.val[i-t-1] = (q.val[i-t-1]-1u) & BNUM_MAX_VAL;
			bnum_bzero(&t1);

			t1.val[0] = (t-1 < 0) ? (0u) : (y.val[t-1]);
			t1.val[1] = y.val[t];
			t1.len = 2;

			bnum_mul_dig(&t1, q.val[i-t-1], &t1);

			t2.val[0] = (i-2 < 0) ? (0u) : (x.val[i-2]);
			t2.val[1] = (i-1 < 0) ? (0u) : (x.val[i-1]);
			t2.val[2] = x.val[i];
			t2.len = 3;
		}
		while (bnum_cmp_u(&t1, &t2) > 0);

		bnum_mul_dig(&y, q.val[i-t-1], &t1);
		bnum_lsh_dig_inpl(&t1, i-t-1);
		bnum_sub(&x, &t1, &x);

		if (x.sign == BNUM_NEG)
		{
			bnum_copy(&y, &t1);
			bnum_lsh_dig_inpl(&t1, i-t-1);
			bnum_add(&x, &t1, &x);
			q.val[i-t-1] = (q.val[i-t-1]-1) & BNUM_MAX_VAL;
		}
	}

	if (c != NULL)
	{
		q.len = a->len+2;
		q.sign = asign * bsign;
		bnum_skip_zeros(&q);
		bnum_copy(&q, c);
	}

	if (d != NULL)
	{
		bnum_skip_zeros(&x);
		x.sign = (BNUM_ZERO(&x)) ? (BNUM_POS):(asign);
		bnum_rsh_bit_inpl(&x, shift);
		bnum_copy(&x, d);
	}

	bnum_clear_multi(&q, &t1, &t2, &x, &y, NULL);
}
