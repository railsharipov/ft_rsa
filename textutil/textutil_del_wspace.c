#include <textutil.h>
#include <libft/string.h>
#include <libft/alloc.h>

// Remove wspace, excluding LF and CR, in character array
// wspace = SP / HT / VT / FF

int textutil_del_wspace(const char *octets, size_t olen, char **p, size_t *psize)
{
	size_t   ix, len;
	char  *res;

	if (NULL == octets || NULL == p) {
		return (SSL_ERR);
	}
	LIBFT_ALLOC(res, olen);
	len = 0;
	ix = 0;

	while (ix < olen) {
		if (!ft_iswspace(octets[ix])) {
			res[len++] = octets[ix++];
		}
		else {
			ix++;
		}
	}
	*p = res;
	*psize = len;

	return (SSL_OK);
}
