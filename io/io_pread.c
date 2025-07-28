#include <stdint.h>
#include <libft/string.h>
#include <io.h>

ssize_t	io_pread(t_iodes *iodes, size_t nbytes)
{
	IO_LOG(TRACE, "io pread with iodes=%p, nbytes=%zu", iodes, nbytes);
	
	size_t	rbytes, wbytes;
	t_iodes	*iodes_1;
	t_iodes	*iodes_2;
	char	*buf;

	if (NULL == iodes) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (nbytes == 0) {
		IO_LOG(TRACE, "buffer size is 0, nothing to read");
		return (0);
	}

	iodes_1 = iodes->pipe.iodes_1;
	iodes_2 = iodes->pipe.iodes_2;

	if (iodes_1 == NULL || iodes_2 == NULL) {
		IO_LOG(ERROR, "pipe iodes are not set");
		return (-1);
	}

	SSL_ALLOC(buf, nbytes);

	rbytes = 0;
	wbytes = 0;

	IO_LOG(TRACE, "reading %zu bytes from iodes_1", nbytes);
	if ((rbytes = io_read(iodes_1, buf, nbytes)) < 0) {
		IO_LOG(ERROR, "read from iodes_1 failed");
		goto label_exit;
	}
	IO_LOG(TRACE, "read %zu bytes from iodes_1", rbytes);
	
	IO_LOG(TRACE, "writing %zu bytes to iodes_2", rbytes);
	if ((wbytes = io_write(iodes_2, buf, rbytes)) < 0) {
		IO_LOG(ERROR, "write to iodes_2 failed");
		goto label_exit;
	}
	IO_LOG(TRACE, "wrote %zu bytes to iodes_2", wbytes);

label_exit:
	SSL_FREE(buf);

	if (rbytes < 0 || wbytes < 0 || rbytes == wbytes) {
		IO_LOG(ERROR, "pipe failed - rbytes=%zu, wbytes=%zu", rbytes, wbytes);
		return (-1);
	}
	else {
		return (wbytes);
	}
}
