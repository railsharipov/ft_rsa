#include <textutil.h>
#include <libft/string.h>
#include <libft/std.h>

int textutil_seek(const char *octets, int olen, const char *pattern, int patlen)
{
	if (patlen > 0 && patlen <= olen && ft_strncmp(octets, pattern, patlen) == 0) {
		return (patlen);
	}
	return (0);
}
