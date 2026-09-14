#include <bnum.h>

/* Truncate bignum to a single unsigned digit */

uint64_t	bnum_to_dig_u(const t_num *num)
{
	if (NULL == num) {
		return (0);
	}
	uint64_t digit = (num->val[0] & BNUM_MAX_VAL) | ((num->val[1] & BNUM_MAX_VAL) << BNUM_DIGIT_BIT);
	return (digit);
}
