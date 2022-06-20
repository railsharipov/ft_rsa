#include <bnum.h>

void	copy_num(const t_num *a, t_num *res, int offset, int len)
{
	int i;

	if (offset + len > res->size)
		increase_num_size(res, offset + len);

	for (i = 0; i < len; i++)
		res->val[i] = a->val[i + offset];

	for (i = len; i < res->size; i++)
		res->val[i] = 0;

	res->len = len;
	res->sign = a->sign;
}
