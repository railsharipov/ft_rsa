#include <textutil.h>
#include <libft/string.h>
#include <libft/alloc.h>

int	textutil_insert_delim(char *octets, size_t olen, char **p, size_t *psize, int delim, int step)
{
	char	*res;
	size_t	ix, iy;

	if (NULL == octets || NULL == p || olen == 0 || psize == NULL || step <= 0) {
		return (SSL_ERR);
	}

	LIBFT_ALLOC(res, olen * 2 + 1);

	for (ix = 0, iy = 0; ix < olen; ix++) {
		if (ix != 0 && ix % (size_t)step == 0) {
			res[iy++] = delim;
		}
		res[iy++] = octets[ix];
	}
	res[iy] = '\0';

	*p = res;
	*psize = iy;

	return (SSL_OK);
}
