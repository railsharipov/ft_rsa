#include <stdint.h>
#include <io.h>
#include <libft.h>
#include <logger.h>

int	io_osbuf(t_iodes *iodes, uint32_t flags, t_ostring *osbuf)
{
	SSL_LOG(TRACE, "io osbuf with flags=0x%x, osbuf=%p", flags, osbuf);

	if (NULL == iodes || NULL == osbuf) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	ft_bzero(iodes, sizeof(t_iodes));

	if (!SSL_FLAG(IO_READ, flags) && !SSL_FLAG(IO_WRITE, flags)) {
		SSL_LOG(ERROR, "invalid flags %#x - neither IO_READ nor IO_WRITE set", flags);
		return (SSL_ERR);
	}
	if (SSL_FLAG(IO_READ, flags) && SSL_FLAG(IO_WRITE, flags)) {
		SSL_LOG(ERROR, "invalid flags %#x - both IO_READ and IO_WRITE set", flags);
		return (SSL_ERR);
	}

	if (SSL_FLAG(IO_READ, flags)) {
		if (NULL == osbuf->content) {
			SSL_LOG(ERROR, "buffer is not specified");
			return (SSL_ERR);
		}
		SSL_LOG(TRACE, "osbuf content=%p, size=%zu", osbuf->content, osbuf->size);
	}

	iodes->osbuf = osbuf;
	iodes->mode = IO_MODE_OSBUF;

	return (SSL_OK);
}
