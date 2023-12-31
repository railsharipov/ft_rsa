#include <bnum.h>

static void	__copy_digs(const t_num *a, t_num *res, int offset, int len);

void	bnum_sqr_karatsuba(const t_num *a, t_num *res)
{
	t_num	a0, a1, t1, t2, a0a0, a1a1;
	int		hlen;

	if (2 * a->len > res->size)
		bnum_increase_size(res, 2 * a->len);

	hlen = a->len >> 1;

	bnum_init_multi(&a0, &a1, &t1, &t2, &a0a0, &a1a1, NULL);

	__copy_digs(a, &a0, 0, hlen);
	__copy_digs(a, &a1, hlen, a->len - hlen);

	bnum_sqr(&a0, &a0a0);
	bnum_sqr(&a1, &a1a1);
	bnum_add_u(&a0, &a1, &t1);
	bnum_sqr(&t1, &t1);

	bnum_add_u(&a0a0, &a1a1, &t2);
	bnum_sub_u(&t1, &t2, &t1);

	bnum_lsh_dig_inpl(&t1, hlen);
	bnum_lsh_dig_inpl(&a1a1, 2*hlen);

	bnum_add_u(&a0a0, &t1, &t1);
	bnum_add_u(&a1a1, &t1, res);

	bnum_clear_multi(&a0, &a1, &t1, &t2, &a0a0, &a1a1, NULL);

	res->len = 2 * a->len;
	res->sign = BNUM_POS;
	bnum_skip_zeros(res);
}

static void	__copy_digs(const t_num *a, t_num *res, int offset, int len)
{
	int i;

	if (len > res->size)
		bnum_increase_size(res, offset + len);

	for (i = 0; i < len; i++)
		res->val[i] = a->val[i + offset];

	res->len = len;
	res->sign = a->sign;
}
