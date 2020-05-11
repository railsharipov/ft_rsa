#include <bnum.h>

void	divmod_num(const t_num *a, const t_num *b, t_num *c, t_num *d)
{
	t_num	q, x, y, t1, t2;
	int		norm, n, t, i;
	int		asign, bsign;

	asign = BNUM_SIGN(a);
	bsign = BNUM_SIGN(b);

	if (BNUM_ZERO(b))
		BNUM_ERROR("division by zero");
	else if (compare_num_u(a, b) < 0)
	{
		if (NULL != d)
			copy_num(a, d, 0, a->len);
		if (NULL != c)
			set_num(c, 0);
		return ;
	}

	init_num(&q);
	init_num(&t1);
	init_num(&t2);
	abs_num(a, &x);
	abs_num(b, &y);

	norm = BNUM_DIGIT_BIT - lmbit_num(&y) % BNUM_DIGIT_BIT;
	lsh_num_b_inpl(&x, norm);
	lsh_num_b_inpl(&y, norm);

	n = x.len-1;
	t = y.len-1;

	lsh_num_d_inpl(&y, n-t);
	while (compare_num(&x, &y) >= 0)
	{
		q.val[n-t] += 1u;
		sub_num(&x, &y, &x);
	}

	rsh_num_d_inpl(&y, n-t);

	for (i = n; i >= t+1; i--)
	{
		if (i > x.len)
			continue ;
		if (x.val[i] == y.val[t])
			q.val[i-t-1] = BNUM_MAX_VAL;
		else
		{
			t_uint128	value;

			value = ((t_uint128) x.val[i]) << BNUM_DIGIT_BIT;
			value |= (t_uint128) x.val[i-1];
			value /= (t_uint128) y.val[t];
			q.val[i-t-1] = value;
		}

		q.val[i-t-1] = (q.val[i-t-1]+1u) & BNUM_MAX_VAL;

		do
		{
			q.val[i-t-1] = (q.val[i-t-1]-1u) & BNUM_MAX_VAL;
			init_num(&t1);

			t1.val[0] = (t-1 < 0) ? (0u) : (y.val[t-1]);
			t1.val[1] = y.val[t];
			t1.len = 2;

			mul_num_d(&t1, q.val[i-t-1], &t1);

			t2.val[0] = (i-2 < 0) ? (0u) : (x.val[i-2]);
			t2.val[1] = (i-1 < 0) ? (0u) : (x.val[i-1]);
			t2.val[2] = x.val[i];
			t2.len = 3;
		}
		while (compare_num_u(&t1, &t2) > 0);

		mul_num_d(&y, q.val[i-t-1], &t1);
		lsh_num_d_inpl(&t1, i-t-1);
		sub_num(&x, &t1, &x);

		if (x.sign == BNUM_NEG)
		{
			copy_num(&y, &t1, 0, y.len);
			lsh_num_d_inpl(&t1, i-t-1);
			add_num(&x, &t1, &x);
			q.val[i-t-1] = (q.val[i-t-1]-1) & BNUM_MAX_VAL;
		}
	}

	if (c != NULL)
	{
		q.len = a->len+2;
		q.sign = asign * bsign;
		skip_zeros(&q);
		copy_num(&q, c, 0, q.len);
	}

	if (d != NULL)
	{
		skip_zeros(&x);
		x.sign = (BNUM_ZERO(&x)) ? (BNUM_POS):(asign);
		rsh_num_b_inpl(&x, norm);
		copy_num(&x, d, 0, x.len);
	}
}
