#include <util/bnum.h>

void	bnum_invmod(const t_num *a, const t_num *b, t_num *res)
{
	t_num	x, y, u, v, ta, tb, tc, td;

	if (BNUM_NEG == b->sign || BNUM_ZERO(b))
		BNUM_ERROR_EXIT("No inverse modulo");

	bnum_init(&x);
	bnum_init(&y);

	bnum_copy(b, &y);
	bnum_divmod(a, b, NULL, &x);

	if (BNUM_EVEN(&x) && BNUM_EVEN(&y))
		BNUM_ERROR_EXIT("No inverse modulo: both numbers are even");

	bnum_init_multi(&u, &v, &ta, &tb, &tc, &td, NULL);

	bnum_copy(&x, &u);
	bnum_copy(&y, &v);
	bnum_set_dig_u(&ta, 1u);
	bnum_set_dig_u(&tb, 0u);
	bnum_set_dig_u(&tc, 0u);
	bnum_set_dig_u(&td, 1u);

	do {

		while (BNUM_EVEN(&u))
		{
			bnum_div2_inpl(&u);
			if (BNUM_ODD(&ta) || BNUM_ODD(&tb))
			{
				bnum_add(&ta, &y, &ta);
				bnum_sub(&tb, &x, &tb);
			}
			bnum_div2_inpl(&ta);
			bnum_div2_inpl(&tb);
		}

		while (BNUM_EVEN(&v))
		{
			bnum_div2_inpl(&v);
			if (BNUM_ODD(&tc) || BNUM_ODD(&td))
			{
				bnum_add(&tc, &y, &tc);
				bnum_sub(&td, &x, &td);
			}
			bnum_div2_inpl(&tc);
			bnum_div2_inpl(&td);
		}

		if (bnum_cmp(&u, &v) >= 0)
		{
			bnum_sub(&u, &v, &u);
			bnum_sub(&ta, &tc, &ta);
			bnum_sub(&tb, &td, &tb);
		}
		else
		{
			bnum_sub(&v, &u, &v);
			bnum_sub(&tc, &ta, &tc);
			bnum_sub(&td, &tb, &td);
		}

	} while (!BNUM_ZERO(&u));

	if (bnum_cmp_dig(&v, 1u) != 0)
		BNUM_ERROR_EXIT("No inverse modulo");

	while (bnum_cmp_dig(&tc, 0u) < 0)
		bnum_add(&tc, b, &tc);

	while (bnum_cmp(&tc, b) >= 0)
		bnum_sub(&tc, b, &tc);

	bnum_copy(&tc, res);
	res->sign = BNUM_POS;

	bnum_clear_multi(&x, &y, &u, &v, &ta, &tb, &tc, &td, NULL);
}
