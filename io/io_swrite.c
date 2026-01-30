#include <libft/string.h>
#include <libft/alloc.h>
#include <io.h>

static ssize_t	__swrite_delim(t_iodes *iodes, const char *buf, size_t nbytes)
{
	char		wbuf[2*nbytes];
	t_ostring	*osbuf;
	ssize_t		wbytes;
	ssize_t		offset;
	ssize_t		width;
	ssize_t		ix;

	osbuf = iodes->osbuf;

	if (iodes->lwidth <= 0) {
		SSL_LOG(TRACE, "setting default lwidth to 64");
		iodes->lwidth = 64;
	}

	width = (size_t)iodes->lwidth;
	offset = MIN(nbytes, (iodes->seek % width));
	wbytes = 0;
	
	SSL_LOG(TRACE, "width=%zd, offset=%zd, current osbuf size=%zu, seek=%zd", width, offset, osbuf->size, iodes->seek);

	/* Finish last line if it's not [width] bytes long */
	if (offset) {
		SSL_LOG(TRACE, "finishing last line with offset");
		for (; wbytes < (width - offset); nbytes--)
			wbuf[wbytes++] = *buf++;
		if (nbytes > 0) {
			wbuf[wbytes++] = iodes->delim;
		}
	}

	SSL_LOG(TRACE, "writing %zu bytes", nbytes);
	while (nbytes/width > 0) {
		for (ix = 0; ix < width; ix++, nbytes--) {
			wbuf[wbytes++] = *buf++;
		}
		wbuf[wbytes++] = iodes->delim;
	}
	while (nbytes-- > 0) {
		wbuf[wbytes++] = *buf++;
	}

	ft_ostr_append(osbuf, wbuf, wbytes);
	iodes->seek += wbytes;
	
	SSL_LOG(TRACE, "wrote %zd bytes, new osbuf size=%zu, seek=%zd", wbytes, osbuf->size, iodes->seek);
	return (wbytes);
}

static ssize_t __swrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	t_ostring	*osbuf;

	osbuf = iodes->osbuf;

	SSL_LOG(TRACE, "writing %zd bytes to osbuf, current osbuf size=%zu, seek=%zd", nbytes, osbuf->size, iodes->seek);
	ft_ostr_append(osbuf, (char *)buf, nbytes);
	iodes->seek += nbytes;
	
	SSL_LOG(TRACE, "wrote %zd bytes, new osbuf size=%zu, seek=%zd", nbytes, osbuf->size, iodes->seek);
	return (nbytes);
}

ssize_t	io_swrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	SSL_LOG(TRACE, "io swrite with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);
	
	ssize_t	wbytes;

	if (NULL == buf || NULL == iodes) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (NULL == iodes->osbuf) {
		SSL_LOG(ERROR, "osbuf is not set");
		return (-1);
	}
	if (nbytes == 0) {
		SSL_LOG(TRACE, "buffer size is 0, nothing to write");
		return (0);
	}

	if (iodes->delim) {
		SSL_LOG(TRACE, "using delimiter-based write, delim=%d", iodes->delim);
		wbytes = __swrite_delim(iodes, (char *)buf, nbytes);
	}
	else {
		SSL_LOG(TRACE, "using write without delimiter");
		wbytes = __swrite(iodes, (char *)buf, nbytes);
	}

	return (wbytes);
}
