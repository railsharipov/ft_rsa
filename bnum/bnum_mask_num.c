#include <bnum.h>

void  mask_num(t_num *num, const char *octets, size_t osize)
{
  unsigned char *oend, *obeg;
  int           digs, bitcnt, ix;
  t_uint128     bits;

  if (NULL == octets)
    return ;

  if (0 == osize)
  {
    set_num(num, 0);
    return ;
  }

  init_num(num);
  digs = CEIL(CHAR_BIT*osize, BNUM_DIGIT_BIT)/BNUM_DIGIT_BIT;
  obeg = (unsigned char *)(octets);
  oend = obeg+(osize-1);
  bitcnt = 0;
  bits = 0;

  if (digs > BNUM_MAX_DIG)
    BNUM_ERROR("big number size limit exceeded");

  for (ix = 0; (ix < digs) && (bitcnt >= 0); ix++)
  {
    while ((bitcnt < BNUM_INT_BIT) && (oend >= obeg))
    {
      bits |= ((t_uint128)(*oend--)) << bitcnt;
      bitcnt += CHAR_BIT;
    }

    num->val[ix] = bits & BNUM_MAX_VAL;
    bits >>= BNUM_DIGIT_BIT;
    bitcnt -= BNUM_DIGIT_BIT;
  }
  for (; ix < BNUM_MAX_DIG; ix++)
    num->val[ix] = 0;

  num->len = digs;
}
