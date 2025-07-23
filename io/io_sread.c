#include <stdint.h>
#include <libft/string.h>
#include <io.h>

static ssize_t __sread_delim(t_iodes *iodes, char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "entering with nbytes=%zu, delim=%d", nbytes, iodes->delim);
	
	t_ostring	*osbuf;
	size_t		rbytes;
	uint32_t	delim;

	osbuf = iodes->osbuf;

	if (NULL == osbuf || NULL == osbuf->content) {
		IO_LOG(TRACE, "NULL osbuf or content - osbuf=%p", osbuf);
		return (-1);
	}

	nbytes = MIN(nbytes, MAX(0, osbuf->size - iodes->seek));
	delim = (uint32_t)iodes->delim;
	rbytes = 0;
	
	IO_LOG(TRACE, "adjusted nbytes=%zu, osbuf size=%zu, seek=%zd", nbytes, osbuf->size, iodes->seek);

	while ((rbytes < nbytes) && (iodes->seek < osbuf->size)) {
		buf[rbytes] = osbuf->content[iodes->seek++];
		rbytes += IS_NONZERO_INT32(*buf ^ delim);
	}

	IO_LOG(TRACE, "read %zu bytes, returning %zd", rbytes, (ssize_t)rbytes);
	return (rbytes);
}

static ssize_t __sread(t_iodes *iodes, char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "entering with nbytes=%zu", nbytes);
	
	t_ostring	*osbuf;
	size_t		rbytes;

	osbuf = iodes->osbuf;

	if (NULL == osbuf || NULL == osbuf->content) {
		IO_LOG(TRACE, "NULL osbuf or content - osbuf=%p", osbuf);
		return (-1);
	}

	nbytes = MIN(nbytes, MAX(0, osbuf->size - iodes->seek));
	rbytes = 0;
	
	IO_LOG(TRACE, "adjusted nbytes=%zu, osbuf size=%zu, seek=%zd", nbytes, osbuf->size, iodes->seek);

	while (nbytes-- > 0)
		buf[rbytes++] = osbuf->content[iodes->seek++];

	IO_LOG(TRACE, "read %zu bytes, returning %zd", rbytes, (ssize_t)rbytes);
	return (rbytes);
}

ssize_t	io_sread(t_iodes *iodes, char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "entering function with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);
	
	size_t	rbytes;

	if (NULL == iodes || NULL == buf) {
		IO_LOG(TRACE, "NULL parameter detected - iodes=%p, buf=%p", iodes, buf);
		return (-1);
	}

	if (nbytes == 0) {
		IO_LOG(TRACE, "nbytes is 0, returning 0");
		return (0);
	}

	IO_LOG(TRACE, "iodes delim=%d", iodes->delim);
	
	if (iodes->delim) {
		IO_LOG(TRACE, "using delimiter-based read");
		rbytes = __sread_delim(iodes, buf, nbytes);
	}
	else {
		IO_LOG(TRACE, "using standard read");
		rbytes = __sread(iodes, buf, nbytes);
	}

	IO_LOG(TRACE, "returning %zd", (ssize_t)rbytes);
	return (rbytes);
}
