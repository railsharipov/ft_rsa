#include <textutil.h>
#include <libft/string.h>
#include <libft/alloc.h>

// Remove empty lines in character array.
// Since, in linux enviroment, CR and LF are interpreted
// as distinct EOL characters (as opposed to DOS),
// CRLF sequence would be equivalent to a double LF.
// This is not what we want, so all CR are ignored.
// LF = line feed character
// CR = carriage return character

static const int LF = 0xA;

// Delete empty lines in character array

ssize_t textutil_del_empty_lines(const char *in, char *out, size_t len)
{
  	size_t   ix, iy;

	if (NULL == in || NULL == out) {
		return (-1);
	}
	ix = 0;
	iy = 0;
	while (ix < len) {
		if (ft_iseol(in[ix])) {
			out[iy++] = LF;
			while (ix < len && ft_iseol(in[ix])) {
				ix++;
			}
		}
		else {
			out[iy++] = in[ix++];
		}
	}
	return ((ssize_t)iy);
}
