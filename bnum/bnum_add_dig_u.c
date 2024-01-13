#include <util/bnum.h>

void	bnum_add_dig_u(const t_num *a, uint64_t digit, t_num *res)
{
	int 	slen, i;

	slen = a->len+1;

	if (slen > res->size)
		bnum_increase_size(res, slen);

	{
		const uint64_t	*aptr;
		uint64_t		*rptr;
		uint128_t		carry, sum;

		aptr = a->val;
		rptr = res->val;
		carry = (uint128_t)digit;
		for (i = 0; i < slen-1; i++)
		{
			sum = ((uint128_t)*aptr++) + carry;
			*rptr++ = (uint64_t)(sum & BNUM_MAX_VAL);
			carry = sum >> BNUM_DIGIT_BIT;
		}
		*rptr = (uint64_t)carry;
	}
	for (i = slen; i < res->size; i++)
	{
		res->val[i] = 0;
	}
	res->len = slen;
	bnum_skip_zeros(res);
}
