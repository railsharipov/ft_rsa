#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <io.h>

void	io_close_multi(t_iodes *iodes, ...)
{
	IO_LOG(TRACE, "io_close_multi: entering function with first iodes=%p", iodes);
	
	va_list	ap;
	int		closed_count = 0;

	if ((NULL != iodes) && (iodes->fd > 2)) {
		IO_LOG(TRACE, "io_close_multi: closing first fd %d", iodes->fd);
		close(iodes->fd);
		closed_count++;
		IO_LOG(TRACE, "io_close_multi: closed first fd %d", iodes->fd);
	} else if (iodes != NULL) {
		IO_LOG(TRACE, "io_close_multi: skipping first fd %d (stdin/stdout/stderr)", iodes->fd);
	}

	va_start(ap, iodes);

	while (NULL != (iodes = va_arg(ap, t_iodes *))) {
		if (iodes->fd > 2) {
			IO_LOG(TRACE, "io_close_multi: closing fd %d", iodes->fd);
			close(iodes->fd);
			closed_count++;
			IO_LOG(TRACE, "io_close_multi: closed fd %d", iodes->fd);
		} else {
			IO_LOG(TRACE, "io_close_multi: skipping fd %d (stdin/stdout/stderr)", iodes->fd);
		}
	}

	va_end(ap);
	
	IO_LOG(TRACE, "io_close_multi: function completed, closed %d file descriptors", closed_count);
}
