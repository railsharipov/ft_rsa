#include <bnum.h>

int bnum_cmp_u(const t_num *a, const t_num *b)
{
	if (a->len != b->len)
	{
		return (a->len - b->len);
	}
	for (int i = a->len-1; i >= 0; i--)
	{
		if (a->val[i] > b->val[i])
			return (1);
		else if (a->val[i] < b->val[i])
			return (-1);
	}
	return (0);
}
