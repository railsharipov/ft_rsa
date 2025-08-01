#include <textutil.h>
#include <libft/string.h>
#include <libft/std.h>

int textutil_seekf(const char *octets, int olen, char *format, ...)
{
	va_list	ap;
	char	*pattern;
	int		patlen;
	int		match;

	va_start(ap, format);
	patlen = ft_vsprintf(*pattern, format, ap);
	va_end(ap);
	
	match = 0;
	if (patlen > 0 && patlen <= olen) {
		match = ft_strncmp(octets, pattern, patlen);
	}
	SSL_FREE(pattern);

	return (match ? patlen : 0);
}
