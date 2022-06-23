#include <bnum.h>

void	abs_num(const t_num *num, t_num *res)
{
  copy_num(num, res);
  res->sign = BNUM_POS;
}
