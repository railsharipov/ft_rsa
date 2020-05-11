#include <ft_ssl.h>
#include <ssl_rand.h>

static uint64_t  MT[MT_N];
static int       MT_IDX = MT_N+1;

static void __twist(void)
{
  static const uint64_t  xt[2] = { 0ULL, MT_A };
  uint64_t               x;
  int                    ix;

  for (ix = 0; ix < (MT_N-MT_M); ix++)
  {
    x = (MT[ix] & MT_HIMASK) | (MT[ix+1] & MT_LOMASK);
    MT[ix] = MT[ix+MT_M] ^ ((x>>1)^xt[(x>>1)&1ULL]);
  }
  for (; ix < (MT_N-1); ix++)
  {
    x = (MT[ix] & MT_HIMASK) | (MT[ix+1] & MT_LOMASK);
    MT[ix] = MT[ix+(MT_M-MT_N)] ^ ((x>>1)^xt[x&1ULL]);
  }
  x = (MT[MT_N-1] & MT_HIMASK) | (MT[0] & MT_LOMASK);
  MT[MT_N-1] = MT[MT_M] ^ ((x>>1)^xt[x&1ULL]);

  MT_IDX = 0;
}

void  rand_mtw_init(uint64_t seed)
{
  int ix;

  MT_IDX = MT_N;
  MT[0] = seed;

  for (ix = 1; ix < MT_N; ix++)
  {
    MT[ix] = MT_MASK & (MT_F * (MT[ix-1] ^ (MT[ix-1]>>(MT_W-2))) + ix);
  }
}

uint64_t  rand_mtw_extract()
{
  uint64_t  y;

  if (MT_IDX >= MT_N)
  {
    if (MT_IDX > MT_N)
    {
      rand_mtw_init(5489ULL);
    }
    __twist();
  }

  y = MT[MT_IDX];
  y = y ^ ((y >> MT_U) & MT_D);
  y = y ^ ((y << MT_S) & MT_B);
  y = y ^ ((y << MT_T) & MT_C);
  y = y ^ (y >> MT_L);

  MT_IDX += 1;
  return (y & MT_MASK);
}
