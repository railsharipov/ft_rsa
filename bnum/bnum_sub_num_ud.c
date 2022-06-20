#include <bnum.h>

void	sub_num_ud(const t_num *a, uint64_t b, t_num *res)
{
	int i;

	if (b > BNUM_MAX_VAL)
	{
		t_num	tmp;

		init_num_with_size(&tmp, 2);

		tmp.val[0] = b & BNUM_MAX_VAL;
		tmp.val[1] = b >> BNUM_DIGIT_BIT;
		tmp.len = 2;

		sub_num(a, &tmp, res);
		clear_num(&tmp);

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
	skip_zeros(res);
}
