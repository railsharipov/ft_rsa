#include <util/bnum.h>

void	bnum_sub_u(const t_num *a, const t_num *b, t_num *res)
{
	int 	slen, i;

	slen = BNUM_MAX(a->len, b->len);

	{
		const uint64_t	*aptr, *bptr;
		uint64_t		borrow, *rptr;

		aptr = a->val;
		bptr = b->val;
		rptr = res->val;

		borrow = 0;
		for (int i = 0; i < slen; i++)
		{
			*rptr = (*aptr++ - *bptr++) - borrow;
			borrow = *rptr >> (BNUM_INT_BIT-1u);
			*rptr++ &= BNUM_MAX_VAL;
		}
	}

	for (i = slen; i < res->size; i++)
		res->val[i] = 0;

	res->len = slen;
	bnum_skip_zeros(res);
}
