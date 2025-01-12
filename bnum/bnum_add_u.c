#include <bnum.h>

void	bnum_add_u(const t_num *a, const t_num *b, t_num *res)
{
	size_t 	slen, i;

	if (a->len < b->len) {
		BNUM_SWAP_PTR(a, b);
	}
	slen = a->len + 1;

	if (res->size < slen) {
		bnum_increase_size(res, slen);
	}
	{
		const uint64_t	*aptr, *bptr;
		uint64_t		carry, *rptr;

		aptr = a->val;
		bptr = b->val;
		rptr = res->val;
		carry = 0;
		for (i = 0; i < b->len; i++)
		{
			*rptr = *aptr++ + *bptr++ + carry;
			carry = *rptr >> BNUM_DIGIT_BIT;
			*rptr++ &= BNUM_MAX_VAL;
		}
		for (; i < a->len; i++)
		{
			*rptr = *aptr++ + carry;
			carry = *rptr >> BNUM_DIGIT_BIT;
			*rptr++ &= BNUM_MAX_VAL;
		}
		*rptr = carry;
	}

	res->len = slen;
	bnum_skip_zeros(res);
}
