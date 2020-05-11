#include <bnum.h>

void    init_num(t_num *num)
{
  for (int i = 0; i < BNUM_MAX_DIG; i++)
      num->val[i] = 0;
  num->len = 1;
	num->sign = BNUM_POS;
}
