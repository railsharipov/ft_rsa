#include <util/bnum.h>

void	bnum_sub_dig_u(const t_num *a, uint64_t b, t_num *res)
{
	int i;

	if (b > BNUM_MAX_VAL)
	{
		t_num	tmp;

		bnum_init_with_size(&tmp, 2);

		tmp.val[0] = b & BNUM_MAX_VAL;
		tmp.val[1] = b >> BNUM_DIGIT_BIT;
		tmp.len = 2;

		bnum_sub(a, &tmp, res);
		bnum_clear(&tmp);

		return ;
	}

	{
		const uint64_t	*aptr;
		uint64_t		borrow, *rptr;

		aptr = a->val;
		rptr = res->val;

		borrow = b;
		for (int i = 0; i < a->len; i++)
		{
			*rptr = *aptr++ - borrow;
			borrow = *rptr >> (BNUM_INT_BIT-1u);
			*rptr++ &= BNUM_MAX_VAL;
		}
	}

	for (i = a->len; i < res->size; i++)
		res->val[i] = 0;

	res->len = a->len;
	bnum_skip_zeros(res);
}
