#include <bnum.h>

void bnum_inv_bits(const t_num *num, t_num *res)
{
	if (num->len > res->size) {
		bnum_increase_size(res, num->len);
	}
	for (int i = num->len-1; i >= 0; i--) {
		res->val[i] = (~(num->val[i])) & BNUM_MAX_VAL;
	}
}
