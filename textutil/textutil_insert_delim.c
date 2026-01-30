#include <textutil.h>
#include <libft/string.h>
#include <libft/alloc.h>

int	textutil_insert_delim(const char *in, size_t inlen, char **out, size_t *outlen, int delim, int step)
{
	char	*res;
	size_t	ix, iy;

	if (NULL == in || NULL == out || NULL == outlen || inlen == 0 || step <= 0) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	SSL_ALLOC(res, inlen * 2 + 1);

	for (ix = 0, iy = 0; ix < inlen; ix++) {
		if (ix != 0 && ix % (size_t)step == 0) {
			res[iy++] = delim;
		}
		res[iy++] = in[ix];
	}
	res[iy] = '\0';

	*out = res;
	*outlen = iy;

	return (SSL_OK);
}
