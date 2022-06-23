#include <bnum.h>

void	copy_num(const t_num *a, t_num *res)
{
	int i;

	if (a->len > res->size)
		increase_num_size(res, a->len);

	for (i = 0; i < a->len; i++)
		res->val[i] = a->val[i];

	for (i = a->len; i < res->size; i++)
		res->val[i] = 0;

	res->len = a->len;
	res->sign = a->sign;
}
