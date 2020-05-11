#include <ft_ssl.h>
#include <parser.h>

// Remove empty lines in character array.
// Since, in linux enviroment, CR and LF are interpreted
// as distinct EOL characters (as opposed to DOS),
// CRLF sequence would be equivalent to a double LF.
// This is not what we want, so all CR are ignored
// LF = line feed character
// CR = carriage return character

static const int LF = 0xA;
static const int CR = 0xD;

// Delete empty lines in character array

void parser_del_empty_lines(const char *arr, int asize, char **p, int *psize)
{
  int   ix;
  char  *rptr;
  char  *res;

  SSL_CHECK(NULL != arr);
  SSL_CHECK(NULL != p);

	SSL_ALLOC(res, asize);
  rptr = res;

  for (ix = 0; (ix < asize) && (ft_iseol(arr[ix]));)
    ix++;

  for (; ix < asize;)
  {
    while ((ix < asize) && (!ft_iseol(arr[ix])))
      *rptr++ = arr[ix++];

    while ((ix < asize) && (ft_iseol(arr[ix])))
      ix++;

    if (ix < asize)
      *rptr++ = LF;
  }

  *p = res;
  *psize = (int)(rptr - (res));
}
