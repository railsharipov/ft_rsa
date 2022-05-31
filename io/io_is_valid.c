#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_io.h>

int	io_is_valid(t_io *io)
{
	if (NULL == io)
		return (IO_ERROR(INVALID_IO));
	if (NULL == io->func)
		return (IO_ERROR(INVALID_IO));
	
	return SSL_OK;
}
