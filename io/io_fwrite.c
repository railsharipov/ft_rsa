#include <unistd.h>
#include <io.h>

static ssize_t	__fwrite_delim(t_iodes *iodes, const char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "entering with nbytes=%zu, delim=%d, lwidth=%d", nbytes, iodes->delim, iodes->lwidth);
	
	char	tbuf[2*nbytes];
	ssize_t	wbytes;
	ssize_t	offset;
	ssize_t	width;
	ssize_t	ix;

	if (iodes->lwidth <= 0) {
		IO_LOG(TRACE, "setting default lwidth to 64");
		iodes->lwidth = 64;
	}

	width = (size_t)iodes->lwidth;
	offset = MIN(nbytes, (iodes->seek % width));
	wbytes = 0;
	
	IO_LOG(TRACE, "width=%zd, offset=%zd", width, offset);

	/* Finish last line if it's not [width] bytes long */
	if (offset) {
		IO_LOG(TRACE, "finishing last line with offset");
		for (; wbytes < (width - offset); nbytes--)
			tbuf[wbytes++] = *buf++;
		if (nbytes > 0) {
			tbuf[wbytes++] = iodes->delim;
		}
	}

	while (nbytes/width > 0) {
		IO_LOG(TRACE, "writing full width line");
		for (ix = 0; ix < width; ix++, nbytes--)
			tbuf[wbytes++] = *buf++;
		tbuf[wbytes++] = iodes->delim;
	}

	while (nbytes-- > 0)
		tbuf[wbytes++] = *buf++;

	IO_LOG(TRACE, "writing %zd bytes to fd %d", wbytes, iodes->fd);
	wbytes = write(iodes->fd, tbuf, wbytes);
	IO_LOG(TRACE, "write returned %zd bytes", wbytes);

	if (wbytes > 0) {
		iodes->seek += wbytes;
		IO_LOG(TRACE, "updated seek to %zd", iodes->seek);
	}

	IO_LOG(TRACE, "returning %zd", wbytes);
	return (wbytes);
}

static ssize_t __fwrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "writing %zu bytes to fd %d", nbytes, iodes->fd);
	
	ssize_t	wbytes;

	wbytes = write(iodes->fd, buf, nbytes);
	IO_LOG(TRACE, "write returned %zd bytes", wbytes);

	if (wbytes > 0) {
		iodes->seek += wbytes;
		IO_LOG(TRACE, "updated seek to %zd", iodes->seek);
	}

	IO_LOG(TRACE, "returning %zd", wbytes);
	return (wbytes);
}

ssize_t	io_fwrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "entering function with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);
	
	ssize_t	wbytes;

	if (NULL == buf || NULL == iodes) {
		IO_LOG(TRACE, "NULL parameter detected - buf=%p, iodes=%p", buf, iodes);
		return (-1);
	}

	if (nbytes == 0) {
		IO_LOG(TRACE, "nbytes is 0, returning 0");
		return (0);
	}

	IO_LOG(TRACE, "iodes delim=%d", iodes->delim);
	
	if (iodes->delim) {
		IO_LOG(TRACE, "using delimiter-based write");
		wbytes = __fwrite_delim(iodes, (char *)buf, nbytes);
	}
	else {
		IO_LOG(TRACE, "using standard write");
		wbytes = __fwrite(iodes, (char *)buf, nbytes);
	}

	IO_LOG(TRACE, "returning %zd", wbytes);
	return (wbytes);
}
