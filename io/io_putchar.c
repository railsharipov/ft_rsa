#include <ft_ssl.h>
#include <ssl_io.h>

ssize_t io_putchar(t_io * const io, char c)
{
	return (write(io->fd, &c, 1));
}
