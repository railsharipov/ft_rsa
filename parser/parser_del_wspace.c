#include <util/parser.h>
#include <libft/string.h>
#include <libft/alloc.h>

// Remove wspace, excluding LF and CR, in character array
// wspace = SP / HT / VT / FF

void parser_del_wspace(const char *arr, int asize, char **p, int *psize)
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
		while ((ix < asize) && (!ft_iswspace(arr[ix]))) {
			*rptr++ = arr[ix++];
		}
		while ((ix < asize) && (ft_iswspace(arr[ix]))) {
			ix++;
		}
	}
	*p = res;
	*psize = (int)(rptr - (res));
}
