#include <bnum.h>

void	swap_num(t_num *a, t_num *b)
{
	t_num	t;

	t = *a;
	*a = *b;
	*b = t;
}
