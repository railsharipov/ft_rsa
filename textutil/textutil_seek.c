#include <textutil.h>
#include <libft/string.h>
#include <libft/std.h>

ssize_t textutil_seek(const char *in, size_t inlen, const char *pat, size_t patlen)
{
	if (patlen > 0 && patlen <= inlen && ft_strncmp(in, pat, patlen) == 0) {
		return ((ssize_t)patlen);
	}
	return (0);
}

ssize_t textutil_seekf(const char *in, size_t inlen, const char *format, ...)
{
	va_list	ap;
	char	*pat;
	size_t	patlen;
	int		match;

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