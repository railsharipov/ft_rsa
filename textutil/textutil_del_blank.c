#include <textutil.h>
#include <libft/string.h>
#include <libft/alloc.h>

// Remove blank in character array
// blank = HT / SP
// HT = horizontal tab
// SP = space

int textutil_del_blank(const char *in, size_t inlen, char **out, size_t *outlen)
{
	char	*res;
	size_t   ix, iy;

	if (NULL == in || NULL == out || NULL == outlen) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_ALLOC(res, inlen + 1);
	ix = 0;
	iy = 0;
	while (ix < inlen) {
		if (!ft_isblank(in[ix])) {
			res[iy] = in[ix];
			iy++;
		}
		ix++;
	}
	res[iy] = '\0';
	*out = res;
	*outlen = iy;

	return (SSL_OK);
}
