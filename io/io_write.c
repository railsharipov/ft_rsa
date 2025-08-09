#include <string.h>
#include <io.h>
#include <common.h>

ssize_t	io_write(t_iodes *iodes, const char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "io write with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);
	
	if (NULL == iodes || NULL == buf) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}

	if (iodes->mode == IO_MODE_FILDES) {
		IO_LOG(TRACE, "routing to io_fwrite");
		return (io_fwrite(iodes, buf, nbytes));
	}
	else if (iodes->mode == IO_MODE_OSBUF) {
		IO_LOG(TRACE, "routing to io_swrite");
		return (io_swrite(iodes, buf, nbytes));
	}
	else {
		IO_LOG(ERROR, "invalid iodes mode %#x", iodes->mode);
		return (-1);
	}
}
