#include <util/bnum.h>

void	bnum_abs(const t_num *num, t_num *res)
{
  bnum_copy(num, res);
  res->sign = BNUM_POS;
}
