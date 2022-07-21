#include <ft_ssl.h>
#include <ssl_io.h>

ssize_t	io_read(t_iodes *iodes, char *buf, size_t nbytes)
{
	if (NULL == iodes || NULL == buf)
		return (-1);

	if (iodes->mode == IO_MODE_FILDES)
		return (io_fread(iodes, buf, nbytes));
	else if (iodes->mode == IO_MODE_OSBUF)
		return (io_sread(iodes, buf, nbytes));
	else
		return (-1);
}
