#include <ft_ssl.h>
#include <ssl_io.h>

void 	io_copy(t_iodes * const dest, t_iodes * const src)
{
	ft_memcpy(dest, src, sizeof(t_iodes));
}
