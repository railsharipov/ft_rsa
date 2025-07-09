#include <string.h>
#include <io.h>

ssize_t	io_read(t_iodes *iodes, char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "io_read: entering function with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);
	
	if (NULL == iodes || NULL == buf) {
		IO_LOG(TRACE, "io_read: NULL parameter detected - iodes=%p, buf=%p", iodes, buf);
		return (-1);
	}

	IO_LOG(TRACE, "io_read: iodes mode=%d", iodes->mode);
	
	if (iodes->mode == IO_MODE_FILDES) {
		IO_LOG(TRACE, "io_read: routing to io_fread");
		return (io_fread(iodes, buf, nbytes));
	}
	else if (iodes->mode == IO_MODE_OSBUF) {
		IO_LOG(TRACE, "io_read: routing to io_sread");
		return (io_sread(iodes, buf, nbytes));
	}
	else if (iodes->mode == IO_MODE_PIPE) {
		IO_LOG(TRACE, "io_read: routing to io_pread");
		return (io_pread(iodes, nbytes));
	}
	else {
		IO_LOG(TRACE, "io_read: invalid mode %d, returning -1", iodes->mode);
		return (-1);
	}
}