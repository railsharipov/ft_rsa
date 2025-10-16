#include <unistd.h>
#include <io.h>

void	io_fclose(t_iodes * const iodes)
{
	if (NULL == iodes) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return ;
	}

	IO_LOG(TRACE, "io fclose with iodes=%p, fd=%d", iodes, iodes->fd);

	if (iodes->mode == IO_MODE_FILDES) {
		if (iodes->fd > 2) {
			IO_LOG(TRACE, "closing file descriptor %d", iodes->fd);
			close(iodes->fd);
			IO_LOG(TRACE, "file descriptor %d closed", iodes->fd);
		} else {
			IO_LOG(TRACE, "skipping close for file descriptor: %d", iodes->fd);
		}
	} else {
		IO_LOG(TRACE, "skipping close for io mode %#x", iodes->mode);
	}
}
