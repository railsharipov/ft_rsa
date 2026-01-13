#include <textutil.h>
#include <libft/string.h>
#include <libft/std.h>

ssize_t textutil_nreplace(const char *in, size_t inlen, char **res, const char *pat, size_t patlen, const char *rep, size_t replen)
{
	size_t	idx, end, reslen;

	if (NULL == in || NULL == pat || NULL == res) {
		TEXTUTIL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (patlen > inlen) {
		return (-1);
	}
	if ((idx = textutil_find(in, inlen, pat, patlen)) < 0) {
		return (SSL_OK);
	}
	end = idx + patlen;
	reslen = inlen - patlen + replen;
	SSL_ALLOC((*res), reslen + 1);
	ft_memcpy((*res), in, idx);
	ft_memcpy((*res) + idx, rep, replen);
	ft_memcpy((*res) + idx + replen, in + end, inlen - end);
	(*res)[reslen] = '\0';

	return ((ssize_t)reslen);
}