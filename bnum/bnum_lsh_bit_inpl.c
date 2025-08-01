#include <bnum.h>

void	bnum_lsh_bit_inpl(t_num *num, int bits)
{
	int			shifts, bit_shifts, i;
	uint64_t	carry, val;

	if (bits <= 0) {
		return ;
	}
	if (BNUM_ZERO(num)) {
		return ;
	}
	shifts = bits / BNUM_DIGIT_BIT;

	bnum_increase_size(num, num->len + shifts + 1);
	bnum_lsh_dig_inpl(num, shifts);
	bit_shifts = bits % BNUM_DIGIT_BIT;

	carry = 0;
	if (bit_shifts) {
		for (i = shifts; i < num->len; i++) {
			val = (num->val[i] << bit_shifts) | carry;
			carry = num->val[i] >> (BNUM_DIGIT_BIT - bit_shifts);
			num->val[i] = val & BNUM_MAX_VAL;
		}
	}
	if (carry) {
		num->val[i] = carry;
		num->len++;
	}
	bnum_skip_zeros(num);
}
