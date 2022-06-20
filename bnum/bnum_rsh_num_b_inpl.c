#include <bnum.h>

void	rsh_num_b_inpl(t_num *num, int bits)
{
	int			shifts;
	int			bit_shifts;
	uint64_t	carry;
	uint64_t	val;

	if (bits <= 0)
		return ;

	shifts = bits / BNUM_DIGIT_BIT;

	if (shifts >= num->len)
	{
		set_num_d(num, 0);
		return ;
	}

	rsh_num_d_inpl(num, shifts);
	bit_shifts = bits % BNUM_DIGIT_BIT;

	if (bit_shifts)
	{
		carry = 0;
		for (int i = num->len-1; i >= 0; i--)
		{
			val = (num->val[i] >> bit_shifts) | carry;
			carry = (num->val[i] << (BNUM_DIGIT_BIT - bit_shifts)) & BNUM_MAX_VAL;
			num->val[i] = val & BNUM_MAX_VAL;
		}
	}
	skip_zeros(num);
}
