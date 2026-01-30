#include <string.h>
#include <io.h>

ssize_t	io_read(t_iodes *iodes, char *buf, size_t nbytes)
{
	SSL_LOG(TRACE, "io read with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);

	if (NULL == iodes || NULL == buf) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}

	if (iodes->mode == IO_MODE_FILDES) {
		SSL_LOG(TRACE, "routing to io_fread");
		return (io_fread(iodes, buf, nbytes));
	}
	else if (iodes->mode == IO_MODE_OSBUF) {
		SSL_LOG(TRACE, "routing to io_sread");
		return (io_sread(iodes, buf, nbytes));
	}
	else if (iodes->mode == IO_MODE_PIPE) {
		SSL_LOG(TRACE, "passing read through pipe");
		return (io_read(iodes->pipe.iodes_out, buf, nbytes));
	}
	else {
		SSL_LOG(ERROR, "invalid iodes mode %#x", iodes->mode);
		return (-1);
	}
}