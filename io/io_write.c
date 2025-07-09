#include <string.h>
#include <io.h>

ssize_t	io_write(t_iodes *iodes, const char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "io_write: entering function with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);
	
	if (NULL == iodes || NULL == buf) {
		IO_LOG(TRACE, "io_write: NULL parameter detected - iodes=%p, buf=%p", iodes, buf);
		return (-1);
	}

	IO_LOG(TRACE, "io_write: iodes mode=%d", iodes->mode);
	
	if (iodes->mode == IO_MODE_FILDES) {
		IO_LOG(TRACE, "io_write: routing to io_fwrite");
		return (io_fwrite(iodes, buf, nbytes));
	}
	else if (iodes->mode == IO_MODE_OSBUF) {
		IO_LOG(TRACE, "io_write: routing to io_swrite");
		return (io_swrite(iodes, buf, nbytes));
	}
	else {
		IO_LOG(TRACE, "io_write: invalid mode %d, returning -1", iodes->mode);
		return (-1);
	}
}
