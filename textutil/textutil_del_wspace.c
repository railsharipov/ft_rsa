#include <textutil.h>
#include <libft/string.h>
#include <libft/alloc.h>

// Remove wspace, excluding LF and CR, in character array
// wspace = SP / HT / VT / FF

ssize_t textutil_del_wspace(const char *in, char *out, size_t len)
{
	size_t   ix, iy;

	if (NULL == in || NULL == out) {
		return (-1);
	}
	ix = 0;
	iy = 0;
	while (ix < len) {
		if (!ft_iswspace(in[ix])) {
			out[iy] = in[ix];
			iy++;
		}
		ix++;
	}
	return ((ssize_t)iy);
}
