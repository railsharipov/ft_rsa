#include <util/parser.h>
#include <libft/string.h>
#include <libft/alloc.h>

// Remove blank in character array
// blank = HT / SP
// HT = horizontal tab
// SP = space

void parser_del_blank(const char *arr, int asize, char **p, int *psize)
{
	int   ix;
	char  *rptr;
	char  *res;

	if (NULL == arr || NULL == p) {
		return ;
	}
	LIBFT_ALLOC(res, asize);
  	rptr = res;

	for (ix = 0; ix < asize;) {
		while ((ix < asize) && (!ft_isblank(arr[ix]))) {
			*rptr++ = arr[ix++];
		}
		while ((ix < asize) && (ft_isblank(arr[ix]))) {
			ix++;
		}
	}
	*p = res;
	*psize = (int)(rptr - (res));
}
