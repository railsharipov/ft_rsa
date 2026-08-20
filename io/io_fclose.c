#include <unistd.h>
#include <io.h>
#include <logger.h>

void	io_fclose(t_iodes * const iodes)
{
	if (NULL == iodes) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return ;
	}

	SSL_LOG(TRACE, "io fclose with iodes=%p, fd=%d", iodes, iodes->fd);

	if (iodes->mode == IO_MODE_FILDES) {
		if (iodes->fd > 2) {
			SSL_LOG(TRACE, "closing file descriptor %d", iodes->fd);
			close(iodes->fd);
			SSL_LOG(TRACE, "file descriptor %d closed", iodes->fd);
		} else {
			SSL_LOG(TRACE, "skipping close for file descriptor: %d", iodes->fd);
		}
	} else {
		SSL_LOG(TRACE, "skipping close for io mode %#x", iodes->mode);
	}
}
