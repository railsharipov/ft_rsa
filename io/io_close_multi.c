#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <util/io.h>

void	io_close_multi(t_iodes *iodes, ...)
{
	va_list	ap;

	if ((NULL != iodes) && (iodes->fd > 2))
		close(iodes->fd);

	va_start(ap, iodes);

	while (NULL != (iodes = va_arg(ap, t_iodes *)))
	{
		if (iodes->fd > 2)
			close(iodes->fd);
	}

	va_end(ap);
}
