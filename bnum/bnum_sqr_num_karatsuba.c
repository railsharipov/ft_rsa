#include <bnum.h>

void	sqr_num_karatsuba(const t_num *a, t_num *res)
{
	t_num	a0, a1, t1, t2, a0a0, a1a1;
	int		hlen;

	if (2 * a->len > res->size)
		increase_num_size(res, 2 * a->len);

	hlen = a->len >> 1;

	init_num_multi(&a0, &a1, &t1, &t2, &a0a0, &a1a1, NULL);

	copy_num(a, &a0, 0, hlen);
	copy_num(a, &a1, hlen, a->len - hlen);

	sqr_num(&a0, &a0a0);
	sqr_num(&a1, &a1a1);
	add_num_u(&a0, &a1, &t1);
	sqr_num(&t1, &t1);

	add_num_u(&a0a0, &a1a1, &t2);
	sub_num_u(&t1, &t2, &t1);

	lsh_num_d_inpl(&t1, hlen);
	lsh_num_d_inpl(&a1a1, 2*hlen);

	add_num_u(&a0a0, &t1, &t1);
	add_num_u(&a1a1, &t1, res);

	clear_num_multi(&a0, &a1, &t1, &t2, &a0a0, &a1a1, NULL);

	res->len = 2 * a->len;
	res->sign = BNUM_POS;
	skip_zeros(res);
}
