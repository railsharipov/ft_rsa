#include <textutil.h>
#include <libft/string.h>
#include <libft/std.h>

int textutil_nreplace(const char *octets, size_t olen, char **res, size_t *rsize, const char *pattern, size_t patlen, const char *rep, size_t replen)
{
	char 	*buf;
	size_t	idx, end, len;

	if (NULL == pattern || NULL == octets || NULL == res || NULL == rsize) {
		TEXTUTIL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (patlen > olen) {
		return (SSL_OK);
	}
	if ((idx = textutil_find(octets, olen, pattern, patlen)) < 0) {
		return (SSL_OK);
	}
	end = idx + patlen;
	len = olen - patlen + replen;
	SSL_ALLOC(buf, len + 1);
	ft_memcpy(buf, octets, idx);
	ft_memcpy(buf + idx, rep, replen);
	ft_memcpy(buf + idx + replen, octets + end, olen - end);
	buf[len] = '\0';
	*res = buf;
	*rsize = len;

	return (SSL_OK);
}