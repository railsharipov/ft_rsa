#include <bnum.h>

void	exp2_num(t_num *res, int n)
{
	if (n < 0)
		BNUM_ERROR("negative exponent is not supported");

	if (n/BNUM_DIGIT_BIT+1 > BNUM_MAX_DIG)
		BNUM_ERROR("big number size limit exceeded");

	init_num(res);
	res->val[n/BNUM_DIGIT_BIT] |= ((uint64_t)1 << (n % BNUM_DIGIT_BIT));
	res->len = n/BNUM_DIGIT_BIT+1;
	res->sign = BNUM_POS;
}
