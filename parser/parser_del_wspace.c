#include <ft_ssl.h>
#include <parser.h>

// Remove wspace, excluding LF and CR, in character array
// wspace = SP / HT / VT / FF

void parser_del_wspace(const char *arr, int asize, char **p, int *psize)
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
    while ((ix < asize) && (!ft_iswspace(arr[ix])))
      *rptr++ = arr[ix++];

    while ((ix < asize) && (ft_iswspace(arr[ix])))
      ix++;
  }

  *p = res;
  *psize = (int)(rptr - (res));
}
