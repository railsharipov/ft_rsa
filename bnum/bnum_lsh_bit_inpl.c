#include <util/bnum.h>

void	bnum_lsh_bit_inpl(t_num *num, int bits)
{
	int			shifts, bit_shifts, slen;
	uint64_t	carry, val;

	if (bits <= 0)
		return ;

	shifts = bits / BNUM_DIGIT_BIT;

	if (shifts >= num->size)
	{
		bnum_set_dig_u(num, 0);
		return ;
	}

	bnum_lsh_dig_inpl(num, shifts);
	slen = BNUM_MIN(num->size, num->len+1);
	bit_shifts = bits % BNUM_DIGIT_BIT;

	if (bit_shifts)
	{
		carry = 0;
		for (int i = shifts; i < slen; i++)
		{
			val = (num->val[i] << bit_shifts) | carry;
			carry = num->val[i] >> (BNUM_DIGIT_BIT - bit_shifts);
			num->val[i] = val & BNUM_MAX_VAL;
		}
	}

	num->len = slen;
	bnum_skip_zeros(num);
}
