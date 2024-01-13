#include <util/parser.h>
#include <libft/string.h>
#include <libft/alloc.h>

void	parser_insert_delim(char **arr, size_t *asize, int delim, int step)
{
	char	*rptr;
	char	*res;
	int		rsize;
	int		ix;
	int		iy;

	if (NULL == arr || NULL == *arr || NULL == asize || *asize == 0 || step <= 0) {
		return ;
	}
	rsize = *asize + (*asize / step) + (int)(*asize % step != 0);
	LIBFT_ALLOC(res, rsize);
	rptr = res;

	for (ix = 0; ix < *asize;) {
		iy = 0;
		while ((ix < *asize) && (iy < step)) {
			*rptr++ = (*arr)[ix++];
			iy++;
		}
		*rptr++ = delim;
	}
	LIBFT_FREE(*arr);
	*arr = res;
	*asize = rsize;
}
