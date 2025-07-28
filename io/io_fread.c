#include <unistd.h>
#include <io.h>

static ssize_t __fread_delim(t_iodes *iodes, char *buf, size_t nbytes)
{
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

		if (rbytes < 0) {
			int err = errno;
			IO_LOG(ERROR, "read error: %s (errno=%d)", strerror(err), err);
			break ;
		}
		if (rbytes == 0) {
			IO_LOG(TRACE, "no bytes read from buffer, stop reading");
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

	return ((rbytes >= 0) ? (tbytes) : (-1));
}

static ssize_t __fread(t_iodes *iodes, char *buf, size_t nbytes)
{
	ssize_t	rbytes;

	rbytes = read(iodes->fd, buf, nbytes);

	if (rbytes < 0) {
		IO_LOG(ERROR, "read error: %s (errno=%d)", strerror(errno), errno);
		return (-1);
	}
	IO_LOG(TRACE, "read %zd bytes from fd %d", rbytes, iodes->fd);

	if (rbytes > 0) {
		iodes->seek += rbytes;
		IO_LOG(TRACE, "updated seek to %zd", iodes->seek);
	}

	return (rbytes);
}

ssize_t io_fread(t_iodes *iodes, char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "io fread with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);
	
	ssize_t	rbytes;

	if (NULL == buf || NULL == iodes) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}

	if (nbytes == 0) {
		IO_LOG(TRACE, "buffer size is 0, nothing to read");
		return (0);
	}

	if (iodes->delim) {
		IO_LOG(TRACE, "using delimiter-based read, delim=%d", iodes->delim);
		rbytes = __fread_delim(iodes, (char *)buf, nbytes);
	}
	else {
		IO_LOG(TRACE, "using read without delimiter");
		rbytes = __fread(iodes, (char *)buf, nbytes);
	}

	return (rbytes);
}
