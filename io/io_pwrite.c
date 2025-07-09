#include <stdint.h>
#include <libft/string.h>
#include <io.h>

ssize_t	io_pwrite(t_iodes *iodes, size_t nbytes)
{
	IO_LOG(TRACE, "io_pwrite: entering function with iodes=%p, nbytes=%zu", iodes, nbytes);
	
	size_t	rbytes, wbytes;
	t_iodes	*iodes_1;
	t_iodes	*iodes_2;
	char	*buf;

	if (NULL == iodes) {
		IO_LOG(TRACE, "io_pwrite: NULL iodes parameter, returning -1");
		return (-1);
	}
	if (nbytes == 0) {
		IO_LOG(TRACE, "io_pwrite: nbytes is 0, returning 0");
		return (0);
	}

	iodes_1 = iodes->pipe.iodes_1;
	iodes_2 = iodes->pipe.iodes_2;
	IO_LOG(TRACE, "io_pwrite: pipe iodes_1=%p, iodes_2=%p", iodes_1, iodes_2);

	if (iodes_1 == NULL || iodes_2 == NULL) {
		IO_LOG(TRACE, "io_pwrite: NULL pipe iodes - iodes_1=%p, iodes_2=%p", iodes_1, iodes_2);
		return (-1);
	}

	SSL_ALLOC(buf, nbytes);
	IO_LOG(TRACE, "io_pwrite: allocated buffer of size %zu", nbytes);

	rbytes = 0;
	wbytes = 0;

	IO_LOG(TRACE, "io_pwrite: reading from iodes_2");
	if ((rbytes = io_read(iodes_2, buf, nbytes)) < 0) {
		IO_LOG(TRACE, "io_pwrite: read from iodes_2 failed, rbytes=%zu", rbytes);
		goto label_exit;
	}
	IO_LOG(TRACE, "io_pwrite: read %zu bytes from iodes_2", rbytes);
	
	IO_LOG(TRACE, "io_pwrite: writing to iodes_1");
	if ((wbytes = io_write(iodes_1, buf, rbytes)) < 0) {
		IO_LOG(TRACE, "io_pwrite: write to iodes_1 failed, wbytes=%zu", wbytes);
		goto label_exit;
	}
	IO_LOG(TRACE, "io_pwrite: wrote %zu bytes to iodes_1", wbytes);

label_exit:
	SSL_FREE(buf);
	IO_LOG(TRACE, "io_pwrite: freed buffer");

	if (rbytes < 0 || wbytes < 0 || rbytes == wbytes) {
		IO_LOG(TRACE, "io_pwrite: operation failed - rbytes=%zu, wbytes=%zu, returning -1", rbytes, wbytes);
		return (-1);
	}
	else {
		IO_LOG(TRACE, "io_pwrite: operation successful, returning %zu", wbytes);
		return (wbytes);
	}
}
