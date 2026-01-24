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

int textutil_del_empty_lines(const char *in, size_t inlen, char **out, size_t *outlen)
{
	char	*res;
  	size_t   ix, iy;

	if (NULL == in || NULL == out || NULL == outlen) {
		TEXTUTIL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_ALLOC(res, inlen + 1);
	ix = 0;
	iy = 0;
	while (ix < inlen) {
		if (ft_iseol(in[ix])) {
			res[iy++] = LF;
			while (ix < inlen && ft_iseol(in[ix])) {
				ix++;
			}
		}
		else {
			res[iy++] = in[ix++];
		}
	}
	res[iy] = '\0';
	*out = res;
	*outlen = iy;

	return (SSL_OK);
}
