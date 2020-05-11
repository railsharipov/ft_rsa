#include <bnum.h>

void	abs_num(const t_num *num, t_num *res)
{
  copy_num(num, res, 0, num->len);
  res->sign = BNUM_POS;
}
