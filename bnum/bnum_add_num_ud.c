#include <bnum.h>

void	add_num_ud(const t_num *a, uint64_t digit, t_num *res)
{
	int 	slen, i;

	slen = a->len+1;

	if (slen > res->size)
		increase_num_size(res, slen);

	{
		const uint64_t	*aptr;
		uint64_t		*rptr;
		t_uint128		carry, sum;

		aptr = a->val;
		rptr = res->val;
		carry = (t_uint128)digit;
		for (i = 0; i < slen-1; i++)
		{
			sum = ((t_uint128)*aptr++) + carry;
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
	skip_zeros(res);
}
