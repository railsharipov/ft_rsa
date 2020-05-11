#include <ft_ssl.h>
#include <ssl_io.h>

void	io_close(t_io * const io)
{
	if (io->fd > 2)
		close(io->fd);
}
