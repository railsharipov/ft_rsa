#include <bnum.h>

void    copy_num(const t_num *a, t_num *res, int offset, int len)
{
  int i;

	if (offset + len > BNUM_MAX_DIG)
		BNUM_ERROR("big number size limit exceeded");
  for (i = 0; i < len; i++)
    res->val[i] = a->val[i + offset];
	for (i = len; i < BNUM_MAX_DIG; i++)
    res->val[i] = 0;
  res->len = len;
	res->sign = BNUM_SIGN(a);
}
