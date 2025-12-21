#include <bytes.h>
#include <libft/std.h>
#include <libft/alloc.h>

int	bytes_new(t_bytes **bytes, t_ostring *ostring)
{
	BYTES_LOG(TRACE, "new bytes with ostring=%p", ostring);

	if (NULL == bytes) {
		BYTES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == ostring) {
		BYTES_LOG(ERROR, "octet string is not specified");
		return (SSL_ERR);
	}

	SSL_ALLOC(*bytes, sizeof(t_bytes));
	(*bytes)->bytes = bytes;
	(*bytes)->seek = 0;
	(*bytes)->delim = 0;
	(*bytes)->lwidth = 0;

	return (SSL_OK);
}
