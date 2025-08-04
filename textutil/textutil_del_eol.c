#include <textutil.h>
#include <libft/string.h>
#include <libft/alloc.h>

// Remove EOL in character array
// EOL = LF / CR

int	textutil_del_eol(const char *octets, size_t olen, char **p, size_t *psize)
{
	char	*rptr;
	char	*res;
	size_t	ix;

	if (NULL == octets || NULL == p) {
		return (SSL_ERR);
	}
	LIBFT_ALLOC(res, olen);
	rptr = res;

	for (ix = 0; ix < olen;) {
		while ((ix < olen) && (!ft_iseol(octets[ix]))) {
			*rptr++ = octets[ix++];
		}
		while ((ix < olen) && (ft_iseol(octets[ix]))) {
			ix++;
		}
	}
	*p = res;
	*psize = (size_t)(rptr - res);

	return (SSL_OK);
}
