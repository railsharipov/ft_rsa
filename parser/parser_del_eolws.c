#include <ft_ssl.h>
#include <parser.h>

// Remove wspace in character array
// wspace = LF / CR / SP / HT / VT / FF

void	parser_del_eolws(const char *arr, int asize, char **p, int *psize)
{
	char	*rptr;
	char	*res;
	int		ix;

	SSL_CHECK(NULL != arr);
	SSL_CHECK(NULL != p);

	SSL_ALLOC(res, asize);
	rptr = res;

	for (ix = 0; ix < asize;)
	{
		while ((ix < asize) && (!ft_iseolws(arr[ix])))
		{
			*rptr++ = arr[ix++];
		}
		while ((ix < asize) && (ft_iseolws(arr[ix])))
		{
			ix++;
		}
	}
	*p = res;
	*psize = (int)(rptr - res);
}
