#include <unistd.h>
#include <io.h>

static ssize_t __fread_delim(t_iodes *iodes, char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "entering with nbytes=%zu, delim=%d", nbytes, iodes->delim);
	
	uint32_t	delim;
	ssize_t		rbytes;
	ssize_t		tbytes;
	char		tbuf[nbytes];
	char 		*tbufptr;

	delim = (uint32_t)iodes->delim;

	tbytes = 0;
	while (tbytes < nbytes) {
		tbufptr = tbuf;
		rbytes = read(iodes->fd, tbuf, nbytes-tbytes);
		IO_LOG(TRACE, "read %zd bytes from fd %d", rbytes, iodes->fd);

		if (rbytes <= 0) {
			IO_LOG(TRACE, "read returned %zd, breaking", rbytes);
			break ;
		}

		iodes->seek += rbytes;
		IO_LOG(TRACE, "updated seek to %zd", iodes->seek);

		while (rbytes > 0) {
			buf[tbytes] = *tbufptr;
			// if character is delimiter, do not increment
			tbytes += (size_t)IS_NONZERO_INT32(*tbufptr ^ delim);
			tbufptr++;
			rbytes--;
		}
	}

	ssize_t result = (rbytes >= 0) ? (tbytes) : (-1);
	IO_LOG(TRACE, "returning %zd", result);
	return (result);
}

static ssize_t __fread(t_iodes *iodes, char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "reading %zu bytes from fd %d", nbytes, iodes->fd);
	
	ssize_t	rbytes;

	rbytes = read(iodes->fd, buf, nbytes);
	IO_LOG(TRACE, "read returned %zd bytes", rbytes);

	if (rbytes > 0) {
		iodes->seek += rbytes;
		IO_LOG(TRACE, "updated seek to %zd", iodes->seek);
	}

	IO_LOG(TRACE, "returning %zd", rbytes);
	return (rbytes);
}

ssize_t io_fread(t_iodes *iodes, char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "entering function with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);
	
	ssize_t	rbytes;

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
		IO_LOG(TRACE, "using delimiter-based read");
		rbytes = __fread_delim(iodes, (char *)buf, nbytes);
	}
	else {
		IO_LOG(TRACE, "using standard read");
		rbytes = __fread(iodes, (char *)buf, nbytes);
	}

	IO_LOG(TRACE, "returning %zd", rbytes);
	return (rbytes);
}
