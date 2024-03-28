#include <util/bnum.h>

int bnum_lmbit(const t_num *num)
{
	int			setbit, i;
	uint64_t	lmint;

	if (num->len == 0) {
		return (0);
	}

	for (i = num->len-1; (i > 0) && (num->val[i] == 0);)
		i--;

	lmint = num->val[i];

	for (setbit = BNUM_DIGIT_BIT; (setbit > 0) && !((lmint >> (setbit-1)) & 0x1);)
		setbit--;

	return (setbit + BNUM_DIGIT_BIT * i);
}
