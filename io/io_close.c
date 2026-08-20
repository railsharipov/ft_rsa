#include <unistd.h>
#include <logger.h>
#include <io.h>

void	io_close(t_iodes * const iodes)
{
	SSL_LOG(TRACE, "entering function with iodes=%p, fd=%d", iodes, iodes ? iodes->fd : -1);

	if (iodes->fd > 2) {
		SSL_LOG(TRACE, "closing file descriptor %d", iodes->fd);
		close(iodes->fd);
		SSL_LOG(TRACE, "file descriptor %d closed", iodes->fd);
	} else {
		SSL_LOG(TRACE, "skipping close for fd %d (stdin/stdout/stderr)", iodes->fd);
	}

	SSL_LOG(TRACE, "function completed");
}
