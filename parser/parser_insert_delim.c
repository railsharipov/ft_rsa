#include <ft_ssl.h>
#include <parser.h>

void	parser_insert_delim(char **arr, int *asize, int delim, int step)
{
	char	*rptr;
	char	*res;
	int		rsize;
	int		ix;
	int		iy;

	SSL_CHECK(NULL != arr);
	SSL_CHECK(NULL != asize);

	if ((step <= 0) || (*asize <= 0) || (NULL == *arr))
	{
		return ;
	}
	rsize = *asize + (*asize / step) + (int)(*asize % step != 0);
	SSL_ALLOC(res, rsize);
	rptr = res;

	for (ix = 0; ix < *asize;)
	{
		iy = 0;
		while ((ix < *asize) && (iy < step))
		{
			*rptr++ = (*arr)[ix++];
			iy++;
		}
		*rptr++ = delim;
	}
	SSL_FREE(*arr);
	*arr = res;
	*asize = rsize;
}
