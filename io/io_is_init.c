#include <ft_ssl.h>
#include <ssl_io.h>

int	io_is_init(t_iodes *iodes)
{
	if (NULL == iodes)
		return (0);

	return (iodes->mode != 0);
}
