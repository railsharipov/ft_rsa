#include <textutil.h>
#include <libft.h>
#include <logger.h>

ssize_t textutil_seek(const char *in, size_t inlen, const char *pat, size_t patlen)
{
	if (NULL == in || NULL == pat) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	if (patlen == 0 || patlen > inlen) {
		return (0);
	}
	if (ft_strneq(in, pat, patlen)) {
		return (patlen);
	}
	return (0);
}

ssize_t textutil_seekf(const char *in, size_t inlen, const char *format, ...)
{
	va_list	ap;
	char	*pat;
	size_t	patlen;
	int		match;

	if (NULL == in || NULL == format) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	va_start(ap, format);
	patlen = ft_vsprintf(&pat, format, ap);
	va_end(ap);

	match = 0;
	if (patlen > 0 && patlen <= inlen) {
		match = ft_strncmp(in, pat, patlen) == 0;
	}
	SSL_FREE(pat);

	return (match ? (ssize_t)patlen : 0);
}

ssize_t textutil_bseekf(const char *in, size_t inlen, const char *format, ...)
{
	va_list	ap;
	char	*pat;
	size_t	patlen;
	int		match;

	if (NULL == in || NULL == format) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	va_start(ap, format);
	patlen = ft_vsprintf(&pat, format, ap);
	va_end(ap);

	match = 0;
	if (patlen > 0 && patlen <= inlen) {
		match = ft_strncmp(in, pat, patlen) == 0;
	}
	SSL_FREE(pat);

	return (match ? (ssize_t)patlen : 0);
}
