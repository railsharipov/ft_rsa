#include <unistd.h>
#include <io.h>

void	io_close(t_iodes * const iodes)
{
	IO_LOG(TRACE, "io_close: entering function with iodes=%p, fd=%d", iodes, iodes ? iodes->fd : -1);
	
	if (iodes->fd > 2) {
		IO_LOG(TRACE, "io_close: closing file descriptor %d", iodes->fd);
		close(iodes->fd);
		IO_LOG(TRACE, "io_close: file descriptor %d closed", iodes->fd);
	} else {
		IO_LOG(TRACE, "io_close: skipping close for fd %d (stdin/stdout/stderr)", iodes->fd);
	}
	
	IO_LOG(TRACE, "io_close: function completed");
}
