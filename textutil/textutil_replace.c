#include <textutil.h>
#include <libft/string.h>
#include <libft/std.h>

int textutil_nreplace(const char *in, size_t inlen, char **out, size_t *outlen, const char *pat, size_t patlen, const char *rep, size_t replen)
{
	char	*res;
	size_t	idx, end, reslen;

	if (NULL == in || NULL == out || NULL == outlen || NULL == pat || NULL == rep) {
		TEXTUTIL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (patlen > inlen) {
		return (SSL_ERR);
	}
	if ((idx = textutil_find(in, inlen, pat, patlen)) < 0) {
		return (SSL_ERR);
	}
	end = idx + patlen;
	reslen = inlen - patlen + replen;
	SSL_ALLOC(res, reslen + 1);
	ft_memcpy(res, in, idx);
	ft_memcpy(res + idx, rep, replen);
	ft_memcpy(res + idx + replen, in + end, inlen - end);
	res[reslen] = '\0';

	*out = res;
	*outlen = reslen;

	return (SSL_OK);
}