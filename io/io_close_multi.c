#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <io.h>
#include <logger.h>

void	io_close_multi(t_iodes *iodes, ...)
{
	SSL_LOG(TRACE, "entering function with first iodes=%p", iodes);

	va_list	ap;
	int		closed_count = 0;

	if ((NULL != iodes) && (iodes->fd > 2)) {
		SSL_LOG(TRACE, "closing first fd %d", iodes->fd);
		close(iodes->fd);
		closed_count++;
		SSL_LOG(TRACE, "closed first fd %d", iodes->fd);
	} else if (iodes != NULL) {
		SSL_LOG(TRACE, "skipping first fd %d (stdin/stdout/stderr)", iodes->fd);
	}

	va_start(ap, iodes);

	while (NULL != (iodes = va_arg(ap, t_iodes *))) {
		if (iodes->fd > 2) {
			SSL_LOG(TRACE, "closing fd %d", iodes->fd);
			close(iodes->fd);
			closed_count++;
			SSL_LOG(TRACE, "closed fd %d", iodes->fd);
		} else {
			SSL_LOG(TRACE, "skipping fd %d (stdin/stdout/stderr)", iodes->fd);
		}
	}

	va_end(ap);

	SSL_LOG(TRACE, "function completed, closed %d file descriptors", closed_count);
}
