#include <textutil.h>
#include <libft.h>
#include <logger.h>

// Remove wspace in character array
// wspace = LF / CR / SP / HT / VT / FF

int textutil_del_eolws(const char *in, size_t inlen, char **out, size_t *outlen)
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
		if (!ft_iseolws(in[ix])) {
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
