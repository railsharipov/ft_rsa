#include <textutil.h>
#include <libft/string.h>
#include <libft/alloc.h>

// Remove EOL in character array
// EOL = LF / CR

ssize_t textutil_del_eol(const char *in, char *out, size_t len)
{
	size_t   ix, iy;

	if (NULL == in || NULL == out) {
		return (-1);
	}
	ix = 0;
	iy = 0;
	while (ix < len) {
		if (!ft_iseol(in[ix])) {
			out[iy] = in[ix];
			iy++;
		}
		ix++;
	}
	return ((ssize_t)iy);
}
