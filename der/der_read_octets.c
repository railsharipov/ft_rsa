#include <ft_ssl.h>
#include <ssl_der.h>
#include <ssl_io.h>

ssize_t	der_read_octets(char *content, size_t size, t_iodes *iodes)
{
	ssize_t	rbytes;

	if (NULL == content || NULL == iodes)
		return (-1);

	if (size == 0)
		return (0);

	rbytes = io_read(iodes, content, size);

	if (rbytes != size)
		return (-1);

	return (rbytes);
}
