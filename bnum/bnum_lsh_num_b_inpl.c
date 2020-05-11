#include <bnum.h>

void	lsh_num_b_inpl(t_num *num, int bits)
{
	int        shifts, bit_shifts, slen;
	uint64_t   carry, value;

	if (bits <= 0)
  {
    return ;
  }
	shifts = bits / BNUM_DIGIT_BIT;
	if (shifts >= BNUM_MAX_DIG)
	{
		set_num(num, 0);
		return ;
	}
	lsh_num_d_inpl(num, shifts);
	slen = BNUM_MIN(BNUM_MAX_DIG, num->len+1);
	bit_shifts = bits % BNUM_DIGIT_BIT;
	if (bit_shifts)
	{
		carry = 0;
		for (int i = shifts; i < slen; i++)
		{
			value = (num->val[i] << bit_shifts) | carry;
			carry = num->val[i] >> (BNUM_DIGIT_BIT - bit_shifts);
			num->val[i] = value & BNUM_MAX_VAL;
		}
	}
	num->len = slen;
	skip_zeros(num);
}
