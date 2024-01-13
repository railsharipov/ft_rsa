#include <util/bnum.h>

void	bnum_swap(t_num *a, t_num *b)
{
	t_num	t;

	t = *a;
	*a = *b;
	*b = t;
}
