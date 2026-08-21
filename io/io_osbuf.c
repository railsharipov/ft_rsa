#include <stdint.h>
#include <io.h>
#include <libft.h>
#include <logger.h>

int	io_osbuf(t_iodes *iodes, uint32_t flags, t_ostring *osbuf)
{
	SSL_LOG(TRACE, "io osbuf with flags=0x%x, osbuf=%p", flags, osbuf);

	if (NULL == iodes || NULL == osbuf) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	ft_bzero(iodes, sizeof(t_iodes));

	if (!SSL_FLAG(IO_READ, flags) && !SSL_FLAG(IO_WRITE, flags)) {
		SSL_LOG(ERROR, "invalid flags %#x - neither IO_READ nor IO_WRITE set", flags);
		return (SSL_ERR);
	}
	if (SSL_FLAG(IO_READ, flags) && SSL_FLAG(IO_WRITE, flags)) {
		SSL_LOG(ERROR, "invalid flags %#x - both IO_READ and IO_WRITE set", flags);
		return (SSL_ERR);
	}

	if (SSL_FLAG(IO_READ, flags)) {
		if (NULL == osbuf->content) {
			SSL_LOG(ERROR, "buffer is not specified");
			return (SSL_ERR);
		}
		SSL_LOG(TRACE, "osbuf content=%p, size=%zu", osbuf->content, osbuf->size);
	}

	iodes->osbuf = osbuf;
	iodes->mode = IO_MODE_OSBUF;

	return (SSL_OK);
}

static ssize_t __sread_delim(t_iodes *iodes, char *buf, size_t nbytes)
{
	t_ostring	*osbuf;
	size_t		rbytes;
	uint32_t	delim;

	osbuf = iodes->osbuf;
	nbytes = MIN(nbytes, MAX(0, osbuf->size - iodes->seek));
	delim = (uint32_t)iodes->delim;
	rbytes = 0;

	SSL_LOG(TRACE, "reading %zu bytes, osbuf size=%zu, seek=%zd", nbytes, osbuf->size, iodes->seek);

	while ((rbytes < nbytes) && (iodes->seek < osbuf->size)) {
		buf[rbytes] = osbuf->content[iodes->seek++];
		rbytes += IS_NONZERO_INT32(*buf ^ delim);
	}

	return (rbytes);
}

static ssize_t __sread(t_iodes *iodes, char *buf, size_t nbytes)
{
	t_ostring	*osbuf;
	size_t		rbytes;

	osbuf = iodes->osbuf;
	nbytes = MIN(nbytes, MAX(0, osbuf->size - iodes->seek));
	rbytes = 0;

	SSL_LOG(TRACE, "reading %zu bytes, osbuf size=%zu, seek=%zd", nbytes, osbuf->size, iodes->seek);

	while (nbytes-- > 0)
		buf[rbytes++] = osbuf->content[iodes->seek++];

	return (rbytes);
}

ssize_t	io_sread(t_iodes *iodes, char *buf, size_t nbytes)
{
	size_t	rbytes;

	SSL_LOG(TRACE, "io sread with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);

	if (NULL == iodes || NULL == buf) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (NULL == iodes->osbuf || NULL == iodes->osbuf->content) {
		SSL_LOG(ERROR, "osbuf is not set or not initialized");
		return (-1);
	}
	if (nbytes == 0) {
		SSL_LOG(TRACE, "buffer size is 0, nothing to read");
		return (0);
	}

	if (iodes->delim) {
		SSL_LOG(TRACE, "using delimiter-based read, delim=%d", iodes->delim);
		rbytes = __sread_delim(iodes, buf, nbytes);
	}
	else {
		SSL_LOG(TRACE, "using read without delimiter");
		rbytes = __sread(iodes, buf, nbytes);
	}

	return (rbytes);
}

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
