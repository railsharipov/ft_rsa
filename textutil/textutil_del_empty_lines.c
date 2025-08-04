#include <textutil.h>
#include <libft/string.h>
#include <libft/alloc.h>

// Remove empty lines in character array.
// Since, in linux enviroment, CR and LF are interpreted
// as distinct EOL characters (as opposed to DOS),
// CRLF sequence would be equivalent to a double LF.
// This is not what we want, so all CR are ignored
// LF = line feed character
// CR = carriage return character

static const int LF = 0xA;

// Delete empty lines in character array

int textutil_del_empty_lines(const char *octets, size_t olen, char **p, size_t *psize)
{
  	size_t   ix, len;
  	char  *res;

	if (NULL == octets || NULL == p) {
		return (SSL_ERR);
	}
	LIBFT_ALLOC(res, olen);
	len = 0;
	ix = 0;

	while (ix < olen) {
		if (ft_iseol(octets[ix])) {
			res[len++] = LF;
			while ((ix < olen) && (ft_iseol(octets[ix]))) {
				ix++;
			}
		}
		else {
			res[len++] = octets[ix++];
		}
	}

	*p = res;
	*psize = len;

	return (SSL_OK);
}
