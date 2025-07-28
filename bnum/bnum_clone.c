#include <bnum.h>

t_num	*bnum_clone(const t_num *src)
{
	int i;
	t_num *dst;

	dst = bnum_create();

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

	return (dst);
}
