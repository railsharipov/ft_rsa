#include <ft_ssl.h>
#include <parser.h>

// Remove blank in character array
// blank = HT / SP
// HT = horizontal tab
// SP = space

void parser_del_blank(const char *arr, int asize, char **p, int *psize)
{
  int   ix;
  char  *rptr;
  char  *res;

  SSL_CHECK(NULL != arr);
  SSL_CHECK(NULL != p);

	SSL_ALLOC(res, asize);
  rptr = res;

  for (ix = 0; ix < asize;)
  {
    while ((ix < asize) && (!ft_isblank(arr[ix])))
      *rptr++ = arr[ix++];

    while ((ix < asize) && (ft_isblank(arr[ix])))
      ix++;
  }

  *p = res;
  *psize = (int)(rptr - (res));
}
