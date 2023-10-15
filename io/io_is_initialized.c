#include <ft_ssl.h>
#include <ssl_io.h>

int	io_is_initialized(t_iodes *iodes)
{
	if (NULL == iodes)
		return (0);

	// The mode flag should be set if I/O is
	return (iodes->mode != 0);
}
