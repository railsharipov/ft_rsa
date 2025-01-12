#include <bnum.h>

void	bnum_lsh_dig_inpl(t_num *num, int shifts)
{
	int	i;

	if (shifts <= 0) {
		return ;
	}
	if (BNUM_ZERO(num)) {
		return ;
	}
	if (num->size < num->len + shifts) {
		bnum_increase_size(num, num->len + shifts);
	}
	num->len = num->len + shifts;

	for (i = num->len-1; i >= shifts; i--) {
		num->val[i] = num->val[i - shifts];
	}
	while (i >= 0) {
		num->val[i--] = 0;
	}
	bnum_skip_zeros(num);
}
