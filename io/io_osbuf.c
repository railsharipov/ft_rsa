#include <stdint.h>
#include <io.h>
#include <libft/string.h>
#include <libft/std.h>

int	io_osbuf(t_iodes *iodes, uint32_t flags, t_ostring *osbuf)
{
	IO_LOG(TRACE, "io osbuf with flags=0x%x, osbuf=%p", flags, osbuf);

	if (NULL == iodes || NULL == osbuf) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	ft_bzero(iodes, sizeof(t_iodes));

	if (!SSL_FLAG(IO_READ, flags) && !SSL_FLAG(IO_WRITE, flags)) {
		IO_LOG(ERROR, "invalid flags %#x - neither IO_READ nor IO_WRITE set", flags);
		return (SSL_ERR);
	}
	if (SSL_FLAG(IO_READ, flags) && SSL_FLAG(IO_WRITE, flags)) {
		IO_LOG(ERROR, "invalid flags %#x - both IO_READ and IO_WRITE set", flags);
		return (SSL_ERR);
	}

	if (SSL_FLAG(IO_READ, flags)) {
		if (NULL == osbuf->content) {
			IO_LOG(ERROR, "IO_READ flag set but buffer is not");
			return (SSL_ERR);
		}
		IO_LOG(TRACE, "osbuf content=%p, size=%zu", osbuf->content, osbuf->size);
	}
	
	iodes->osbuf = osbuf;
	iodes->mode = IO_MODE_OSBUF;

	return (SSL_OK);
}
