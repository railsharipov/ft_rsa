#include <bnum.h>

void	mul_num_karatsuba(const t_num *a, const t_num *b, t_num *res)
{
	t_num	a0, a1, b0, b1, a0b0, a1b1, temp;
	int		hlen;

	if (a->len + b->len > BNUM_MAX_DIG)
		BNUM_ERROR("big number size limit exceeded");

	hlen = BNUM_MIN(a->len, b->len) >> 1;

	init_num(&temp);
	copy_num(a, &a0, 0, hlen);
	copy_num(a, &a1, hlen, a->len - hlen);
	copy_num(b, &b0, 0, hlen);
	copy_num(b, &b1, hlen, b->len - hlen);

	mul_num(&a0, &b0, &a0b0);
	mul_num(&a1, &b1, &a1b1);

	add_num_u(&a0, &a1, &a0);
	add_num_u(&b0, &b1, &b0);
	add_num_u(&a0b0, &a1b1, &temp);

	mul_num(&a0, &b0, &a0);
	sub_num_u(&a0, &temp, &temp);

	lsh_num_d_inpl(&a1b1, 2*hlen);
	lsh_num_d_inpl(&temp, hlen);

	add_num_u(&a1b1, &temp, &a1b1);
	add_num_u(&a1b1, &a0b0, res);

	res->len = a->len + b->len;
	res->sign = BNUM_SIGN(a) * BNUM_SIGN(b);
	skip_zeros(res);
}
