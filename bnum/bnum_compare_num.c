#include <bnum.h>

int compare_num(const t_num *a, const t_num *b)
{
	if (a->sign != b->sign)
	{
		if (a->sign > b->sign)
    {
      return (1);
    }
		else
    {
      return (-1);
    }
	}
  if (BNUM_POS == a->sign)
  {
    return (compare_num_u(a, b));
  }
	else
  {
    return (compare_num_u(b, a));
  }
}
