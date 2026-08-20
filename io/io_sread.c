#include <stdint.h>
#include <libft.h>
#include <io.h>
#include <logger.h>

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
