#include <ft_ssl.h>
#include <ssl_der.h>
#include <ssl_io.h>

ssize_t	der_write_octets(char *content, size_t size, t_iodes *iodes)
{
	ssize_t	wbytes;

	if (NULL == content || NULL == iodes)
		return (-1);

	if (size == 0)
		return (0);

	wbytes = io_write(iodes, content, size);

	return (wbytes);
}
