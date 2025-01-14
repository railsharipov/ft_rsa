#include <bnum.h>

void	bnum_sub_u(const t_num *a, const t_num *b, t_num *res)
{
	size_t 	i;

	if (res->size < a->len) {
		bnum_increase_size(res, a->len);
	}
	{
		const uint64_t	*aptr, *bptr;
		uint64_t		borrow, *rptr;

		aptr = a->val;
		bptr = b->val;
		rptr = res->val;

		borrow = 0;
		for (i = 0; i < b->len; i++) {
			*rptr = ((BNUM_BASE + *aptr++) - *bptr++) - borrow;
			borrow = (~(*rptr >> BNUM_DIGIT_BIT)) & 0x1;
			*rptr++ &= BNUM_MAX_VAL;
		}
		for (; i < a->len; i++) {
			*rptr = (BNUM_BASE + *aptr++) - borrow;
			borrow = (~(*rptr >> BNUM_DIGIT_BIT)) & 0x1;
			*rptr++ &= BNUM_MAX_VAL;
		}
	}

	res->len = a->len;
	bnum_skip_zeros(res);
}
