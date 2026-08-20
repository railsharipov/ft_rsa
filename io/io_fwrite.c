#include <unistd.h>
#include <io.h>
#include <logger.h>

static ssize_t	__fwrite_delim(t_iodes *iodes, const char *buf, size_t nbytes)
{
	char	tbuf[2*nbytes];
	ssize_t	wbytes;
	ssize_t	offset;
	ssize_t	width;
	ssize_t	ix;

	if (iodes->lwidth <= 0) {
		SSL_LOG(TRACE, "setting default lwidth to 64");
		iodes->lwidth = 64;
	}

	width = (size_t)iodes->lwidth;
	offset = MIN(nbytes, (iodes->seek % width));
	wbytes = 0;

	SSL_LOG(TRACE, "width=%zd, offset=%zd", width, offset);

	/* Finish last line if it's not [width] bytes long */
	if (offset) {
		SSL_LOG(TRACE, "finishing last line with offset");
		for (; wbytes < (width - offset); nbytes--)
			tbuf[wbytes++] = *buf++;
		if (nbytes > 0) {
			tbuf[wbytes++] = iodes->delim;
		}
	}

	while (nbytes/width > 0) {
		SSL_LOG(TRACE, "writing full width line");
		for (ix = 0; ix < width; ix++, nbytes--)
			tbuf[wbytes++] = *buf++;
		tbuf[wbytes++] = iodes->delim;
	}

	while (nbytes-- > 0)
		tbuf[wbytes++] = *buf++;

	SSL_LOG(TRACE, "writing %zd bytes to fd %d", wbytes, iodes->fd);
	wbytes = write(iodes->fd, tbuf, wbytes);

	if (wbytes < 0) {
		SSL_LOG(ERROR, "write error: %s (errno=%d)", strerror(errno), errno);
		return (-1);
	}
	SSL_LOG(TRACE, "write %zd bytes to fd %d", wbytes, iodes->fd);

	if (wbytes > 0) {
		iodes->seek += wbytes;
		SSL_LOG(TRACE, "updated seek to %zd", iodes->seek);
	}

	return (wbytes);
}

static ssize_t __fwrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	ssize_t	wbytes;

	SSL_LOG(TRACE, "writing %zu bytes to fd %d", nbytes, iodes->fd);
	wbytes = write(iodes->fd, buf, nbytes);

	if (wbytes < 0) {
		SSL_LOG(ERROR, "write error: %s (errno=%d)", strerror(errno), errno);
		return (-1);
	}
	SSL_LOG(TRACE, "write %zd bytes to fd %d", wbytes, iodes->fd);

	if (wbytes > 0) {
		iodes->seek += wbytes;
		SSL_LOG(TRACE, "updated seek to %zd", iodes->seek);
	}

	return (wbytes);
}

ssize_t	io_fwrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	ssize_t	wbytes;

	SSL_LOG(TRACE, "io fwrite with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);

	if (NULL == buf || NULL == iodes) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}

	if (nbytes == 0) {
		SSL_LOG(TRACE, "buffer size is 0, nothing to write");
		return (0);
	}

	if (iodes->delim) {
		SSL_LOG(TRACE, "using delimiter-based write, delim=%d", iodes->delim);
		wbytes = __fwrite_delim(iodes, (char *)buf, nbytes);
	}
	else {
		SSL_LOG(TRACE, "using write without delimiter");
		wbytes = __fwrite(iodes, (char *)buf, nbytes);
	}

	return (wbytes);
}
