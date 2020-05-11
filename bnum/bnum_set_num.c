#include <bnum.h>

void    set_num(t_num *num, uint64_t val)
{
  init_num(num);
  num->val[0] = val;
  num->len = 1;
	num->sign = BNUM_POS;
}
