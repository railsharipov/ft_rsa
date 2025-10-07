#include <stdint.h>
#include <libft/string.h>
#include <io.h>

ssize_t	io_pump(t_iodes *iodes, size_t nbytes)
{
	IO_LOG(TRACE, "io pump with iodes=%p, nbytes=%zu", iodes, nbytes);

	char	buf[IO_BUFSIZE];
	ssize_t	rbytes;
	ssize_t	wbytes;
	t_iodes	*iodes_in;
	t_iodes	*iodes_out;

	if (NULL == iodes) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (iodes->mode != IO_MODE_PIPE) {
		IO_LOG(ERROR, "io_pump can only be used on pipes");
		return (-1);
	}

	iodes_in = iodes->pipe.iodes_in;
	iodes_out = iodes->pipe.iodes_out;

	if (iodes_in == NULL || iodes_out == NULL) {
		IO_LOG(ERROR, "pipe iodes are not set");
		return (-1);
	}

	rbytes = io_read(iodes_in, buf, nbytes > IO_BUFSIZE ? IO_BUFSIZE : nbytes);
	if (rbytes < 0) {
		IO_LOG(ERROR, "pump read from iodes_in failed");
		return (-1);
	}
	if (rbytes == 0) {
		return (0);
	}

	wbytes = io_write(iodes_out, buf, rbytes);
	if (wbytes < 0) {
		IO_LOG(ERROR, "pump write to iodes_out failed");
		return (-1);
	}

	if (wbytes != rbytes) {
		IO_LOG(WARN, "pumped %zu bytes, but only wrote %zu", rbytes, wbytes);
	}
	return (wbytes);
}
