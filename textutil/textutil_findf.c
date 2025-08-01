#include <textutil.h>
#include <libft/string.h>
#include <libft/std.h>

// Find ostring 'pattern' in ostring 'octets'
int textutil_findf(const char *octets, int olen, char *format, ...)
{
	va_list	ap;
	char	*pattern;
	int		patlen;
	int		idx;

	va_start(ap, format);
	patlen = ft_vsprintf(&pattern, format, ap);
	va_end(ap);

	idx = textutil_find(octets, olen, pattern, patlen);
	SSL_FREE(pattern);

	return (idx);
}
