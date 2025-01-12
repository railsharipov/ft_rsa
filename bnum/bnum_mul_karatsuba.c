#include <bnum.h>

static void	__copy_digs(const t_num *a, t_num *res, int offset, int len);

void	bnum_mul_karatsuba(const t_num *a, const t_num *b, t_num *res)
{
	t_num	a0, a1, b0, b1, a0b0, a1b1, temp;
	int		ndig, hlen;

	ndig = a->len + b->len;

	if (ndig > res->size)
		bnum_increase_size(res, ndig);

	hlen = BNUM_MIN(a->len, b->len) >> 1;

	bnum_init_multi(&a0, &a1, &b0, &b1, &a0b0, &a1b1, &temp, NULL);

	__copy_digs(a, &a0, 0, hlen);
	__copy_digs(a, &a1, hlen, a->len - hlen);
	__copy_digs(b, &b0, 0, hlen);
	__copy_digs(b, &b1, hlen, b->len - hlen);

	bnum_mul(&a0, &b0, &a0b0);
	bnum_mul(&a1, &b1, &a1b1);

	bnum_add_u(&a0, &a1, &a0);
	bnum_add_u(&b0, &b1, &b0);
	bnum_add_u(&a0b0, &a1b1, &temp);

	bnum_mul(&a0, &b0, &a0);
	bnum_sub_u(&a0, &temp, &temp);

	bnum_lsh_dig_inpl(&a1b1, 2*hlen);
	bnum_lsh_dig_inpl(&temp, hlen);

	bnum_add_u(&a1b1, &temp, &a1b1);
	bnum_add_u(&a1b1, &a0b0, res);

	bnum_clear_multi(&a0, &a1, &b0, &b1, &a0b0, &a1b1, &temp, NULL);

	res->len = ndig;
	res->sign = a->sign * b->sign;
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
