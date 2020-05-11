#include <ft_ssl.h>
#include <ssl_io.h>

void 	io_copy(t_io * const dest, t_io * const src)
{
	ft_memcpy(dest, src, sizeof(t_io));
}
