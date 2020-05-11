#include <bnum.h>

void	add_num_ud(const t_num *a, uint64_t b, t_num *res)
{
	int 	slen, i;

	slen = a->len+1;

	if (slen > BNUM_MAX_DIG)
		BNUM_ERROR("big number size limit exceeded");

	{
		const uint64_t	*aptr;
		uint64_t		carry, *rptr;

		aptr = a->val;
		rptr = res->val;
		carry = b;
		for (i = 0; i < slen-1; i++)
		{
			*rptr = *aptr++ + carry;
			carry = *rptr >> BNUM_DIGIT_BIT;
			*rptr++ &= BNUM_MAX_VAL;
		}
		*rptr = carry;
	}
	for (i = slen; i < BNUM_MAX_DIG; i++)
  {
    res->val[i] = 0;
  }
	res->len = slen;
	skip_zeros(res);
}
