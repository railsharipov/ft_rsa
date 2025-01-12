#include <bnum.h>

void	bnum_copy(const t_num *src, t_num *dst)
{
	int i;

	if (src->len > dst->size) {
		bnum_increase_size(dst, src->len);
	}
	for (i = 0; i < src->len; i++) {
		dst->val[i] = src->val[i];
	}
	for (i = src->len; i < dst->size; i++) {
		dst->val[i] = 0;
	}
	dst->len = src->len;
	dst->sign = src->sign;
}
