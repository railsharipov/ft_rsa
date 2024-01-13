#include <string.h>
#include <util/io.h>

ssize_t	io_write(t_iodes *iodes, const char *buf, size_t nbytes)
{
	if (NULL == iodes || NULL == buf)
		return (-1);

	if (iodes->mode == IO_MODE_FILDES)
		return (io_fwrite(iodes, buf, nbytes));
	else if (iodes->mode == IO_MODE_OSBUF)
		return (io_swrite(iodes, buf, nbytes));
	else
		return (-1);
}
