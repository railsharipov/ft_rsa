#include <bnum.h>

int     compare_num_d(const t_num *a, uint64_t b)
{
  if (BNUM_NEG == a->sign)
    return (-1);
	if (a->len > 1)
		return (1);
  if (a->val[0] > b)
		return (1);
	else if (a->val[0] < b)
		return (-1);
	else
		return (0);
}
