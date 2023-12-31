#include <bnum.h>

void	invmod_num(const t_num *a, const t_num *b, t_num *res)
{
	t_num	x, y, u, v, ta, tb, tc, td;

	if (BNUM_NEG == b->sign || BNUM_ZERO(b))
		BNUM_ERROR_EXIT("No inverse modulo");

	init_num(&x);
	init_num(&y);

	copy_num(b, &y);
	divmod_num(a, b, NULL, &x);

	if (BNUM_EVEN(&x) && BNUM_EVEN(&y))
		BNUM_ERROR_EXIT("No inverse modulo: both numbers are even");

	init_num_multi(&u, &v, &ta, &tb, &tc, &td, NULL);

	copy_num(&x, &u);
	copy_num(&y, &v);
	set_dig_u(&ta, 1u);
	set_dig_u(&tb, 0u);
	set_dig_u(&tc, 0u);
	set_dig_u(&td, 1u);

	do {

		while (BNUM_EVEN(&u))
		{
			div_num_2d_inpl(&u);
			if (BNUM_ODD(&ta) || BNUM_ODD(&tb))
			{
				add_num(&ta, &y, &ta);
				sub_num(&tb, &x, &tb);
			}
			div_num_2d_inpl(&ta);
			div_num_2d_inpl(&tb);
		}

		while (BNUM_EVEN(&v))
		{
			div_num_2d_inpl(&v);
			if (BNUM_ODD(&tc) || BNUM_ODD(&td))
			{
				add_num(&tc, &y, &tc);
				sub_num(&td, &x, &td);
			}
			div_num_2d_inpl(&tc);
			div_num_2d_inpl(&td);
		}

		if (compare_num(&u, &v) >= 0)
		{
			sub_num(&u, &v, &u);
			sub_num(&ta, &tc, &ta);
			sub_num(&tb, &td, &tb);
		}
		else
		{
			sub_num(&v, &u, &v);
			sub_num(&tc, &ta, &tc);
			sub_num(&td, &tb, &td);
		}

	} while (!BNUM_ZERO(&u));

	if (compare_dig(&v, 1u) != 0)
		BNUM_ERROR_EXIT("No inverse modulo");

	while (compare_dig(&tc, 0u) < 0)
		add_num(&tc, b, &tc);

	while (compare_num(&tc, b) >= 0)
		sub_num(&tc, b, &tc);

	copy_num(&tc, res);
	res->sign = BNUM_POS;

	clear_num_multi(&x, &y, &u, &v, &ta, &tb, &tc, &td, NULL);
}
