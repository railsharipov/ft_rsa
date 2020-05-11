#include <ft_ssl.h>
#include <ssl_io.h>
#include <stdarg.h>

void	io_close_multi(t_io *io, ...)
{
	va_list	ap;

	if ((NULL != io) && (io->fd > 2))
		close(io->fd);

	va_start(ap, io);

	while (NULL != (io = va_arg(ap, t_io *)))
	{
		if (io->fd > 2)
			close(io->fd);
	}

	va_end(ap);
}
