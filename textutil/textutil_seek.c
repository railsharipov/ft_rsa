#include <textutil.h>
#include <libft/string.h>
#include <libft/std.h>

int textutil_seek(const char *octets, size_t olen, const char *pattern, size_t patlen)
{
	if (patlen > 0 && patlen <= olen && ft_strncmp(octets, pattern, patlen) == 0) {
		return (patlen);
	}
	return (0);
}

int textutil_seekf(const char *octets, size_t olen, const char *format, ...)
{
	va_list	ap;
	char	*pattern;
	size_t	patlen;
	int		match;

	va_start(ap, format);
	patlen = ft_vsprintf(&pattern, format, ap);
	va_end(ap);
	
	match = 0;
	if (patlen > 0 && patlen <= olen) {
		match = ft_strncmp(octets, pattern, patlen) == 0;
	}
	SSL_FREE(pattern);

	return (match ? patlen : 0);
}

int textutil_bseekf(const char *octets, size_t olen, const char *format, ...)
{
	va_list	ap;
	char	*pattern;
	// char 	*ext_format;
	size_t	patlen;
	int		match;

	va_start(ap, format);
	patlen = ft_vsprintf(&pattern, format, ap);
	va_end(ap);
	
	match = 0;
	if (patlen > 0 && patlen <= olen) {
		match = ft_strncmp(octets, pattern, patlen) == 0;
	}
	SSL_FREE(pattern);

	return (match ? patlen : 0);
}