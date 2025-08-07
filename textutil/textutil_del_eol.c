#include <textutil.h>
#include <libft/string.h>
#include <libft/alloc.h>

// Remove EOL in character array
// EOL = LF / CR

int	textutil_del_eol(const char *octets, size_t olen, char **p, size_t *psize)
{
	char	*res;
	size_t	ix, len;

	if (NULL == octets || NULL == p) {
		return (SSL_ERR);
	}
	SSL_ALLOC(res, olen);

	len = 0;
	for (ix = 0; ix < olen;) {
		while ((ix < olen) && (!ft_iseol(octets[ix]))) {
			res[len++] = octets[ix++];
		}
		while ((ix < olen) && (ft_iseol(octets[ix]))) {
			ix++;
		}
	}
	*p = res;
	*psize = len;

	return (SSL_OK);
}
