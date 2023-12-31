#include <bnum.h>

static void	__copy_digs(const t_num *a, t_num *res, int offset, int len);

void	mul_num_karatsuba(const t_num *a, const t_num *b, t_num *res)
{
	t_num	a0, a1, b0, b1, a0b0, a1b1, temp;
	int		ndig, hlen;

	ndig = a->len + b->len;

	if (ndig > res->size)
		increase_num_size(res, ndig);

	hlen = BNUM_MIN(a->len, b->len) >> 1;

	init_num_multi(&a0, &a1, &b0, &b1, &a0b0, &a1b1, &temp, NULL);

	__copy_digs(a, &a0, 0, hlen);
	__copy_digs(a, &a1, hlen, a->len - hlen);
	__copy_digs(b, &b0, 0, hlen);
	__copy_digs(b, &b1, hlen, b->len - hlen);

	mul_num(&a0, &b0, &a0b0);
	mul_num(&a1, &b1, &a1b1);

	add_num_u(&a0, &a1, &a0);
	add_num_u(&b0, &b1, &b0);
	add_num_u(&a0b0, &a1b1, &temp);

	mul_num(&a0, &b0, &a0);
	sub_num_u(&a0, &temp, &temp);

	lsh_dig_inpl(&a1b1, 2*hlen);
	lsh_dig_inpl(&temp, hlen);

	add_num_u(&a1b1, &temp, &a1b1);
	add_num_u(&a1b1, &a0b0, res);

	clear_num_multi(&a0, &a1, &b0, &b1, &a0b0, &a1b1, &temp, NULL);

	res->len = ndig;
	res->sign = a->sign * b->sign;
	skip_zeros(res);
}

static void	__copy_digs(const t_num *a, t_num *res, int offset, int len)
{
	int i;

	if (len > res->size)
		increase_num_size(res, offset + len);

	for (i = 0; i < len; i++)
		res->val[i] = a->val[i + offset];

	res->len = len;
	res->sign = a->sign;
}
