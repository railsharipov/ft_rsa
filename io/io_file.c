#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <io.h>
#include <logger.h>
#include <unistd.h>
#include <sys/fcntl.h>

void	io_fclose_multi(t_iodes *iodes, ...)
{
	va_list	ap;

	SSL_LOG(TRACE, "closing multiple file descriptors");
	io_fclose(iodes);

	va_start(ap, iodes);

	while (NULL != (iodes = va_arg(ap, t_iodes *))) {
		io_fclose(iodes);
	}
	va_end(ap);
}


void	io_fclose(t_iodes * const iodes)
{
	if (NULL == iodes) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return ;
	}

	SSL_LOG(TRACE, "io fclose with iodes=%p, fd=%d", iodes, iodes->fd);

	if (iodes->mode == IO_MODE_FILDES) {
		if (iodes->fd > 2) {
			SSL_LOG(TRACE, "closing file descriptor %d", iodes->fd);
			close(iodes->fd);
			SSL_LOG(TRACE, "file descriptor %d closed", iodes->fd);
		} else {
			SSL_LOG(TRACE, "skipping close for file descriptor: %d", iodes->fd);
		}
	} else {
		SSL_LOG(TRACE, "skipping close for io mode %#x", iodes->mode);
	}
}

typedef struct	s_io_param
{
	uint32_t	flag;
	int			fd;
}				t_io_param;

static const t_io_param	T[] = {
	/*	flags				fd				*/
	{	IO_READ|IO_STDIN,	STDIN_FILENO	},
	{	IO_READ|IO_STDOUT,	STDOUT_FILENO	},
	{	IO_READ|IO_FILE,	-1				},
	{	IO_WRITE|IO_STDIN,	STDIN_FILENO	},
	{	IO_WRITE|IO_STDOUT,	STDOUT_FILENO	},
	{	IO_WRITE|IO_FILE,	-1				},
};

static const size_t	TSIZE = sizeof(T)/sizeof(t_io_param);

static t_io_param	*__get_param(uint32_t);

int	io_fopen(t_iodes *iodes, uint32_t flags, const char *filename)
{
	SSL_LOG(TRACE, "io fopen with flags=0x%x, filename=%s", flags, filename ? filename : "");

	t_io_param	*param;

	if (NULL == iodes) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}

	ft_bzero(iodes, sizeof(t_iodes));

	if (NULL == (param = __get_param(flags))) {
		SSL_LOG(ERROR, "invalid flags %#x", flags);
		return (-1);
	}

	iodes->fd = param->fd;

	if (SSL_FLAG(IO_FILE, flags)) {
		if (NULL == filename) {
			SSL_LOG(ERROR, "filename is not specified");
			return (-1);
		}

		SSL_LOG(TRACE, "opening file '%s' with flags 0x%x", filename, flags);
		if (SSL_FLAG(IO_READ, flags)) {
			iodes->fd = open(filename, O_RDONLY, 0644);
			SSL_LOG(TRACE, "opened file for reading, fd=%d", iodes->fd);
		}
		else if (SSL_FLAG(IO_WRITE, flags)) {
			iodes->fd = open(filename, O_TRUNC|O_RDWR|O_CREAT, 0644);
			SSL_LOG(TRACE, "opened file for writing, fd=%d", iodes->fd);
		}
		else {
			SSL_LOG(ERROR, "invalid flags combination");
			return (-1);
		}
	}

	if (iodes->fd < 0) {
		SSL_LOG(ERROR, "file open failed, fd=%d", iodes->fd);
		return (-1);
	}

	iodes->mode = IO_MODE_FILDES;

	return (0);
}
static t_io_param	*__get_param(uint32_t flags)
{
	int	ix;

	ix = 0;
	while (ix < TSIZE) {
		if (T[ix].flag == flags) {
			return ((t_io_param *)T+ix);
		}
		ix++;
	}

	return (NULL);
}

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
		SSL_LOG(TRACE, "read %zd bytes from fd %d", rbytes, iodes->fd);

		if (rbytes < 0) {
			int err = errno;
			SSL_LOG(ERROR, "read error: %s (errno=%d)", strerror(err), err);
			break ;
		}
		if (rbytes == 0) {
			SSL_LOG(TRACE, "no bytes read from buffer, stop reading");
			break ;
		}

		iodes->seek += rbytes;
		SSL_LOG(TRACE, "updated seek to %zd", iodes->seek);

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
		SSL_LOG(ERROR, "read error: %s (errno=%d)", strerror(errno), errno);
		return (-1);
	}
	SSL_LOG(TRACE, "read %zd bytes from fd %d", rbytes, iodes->fd);

	if (rbytes > 0) {
		iodes->seek += rbytes;
		SSL_LOG(TRACE, "updated seek to %zd", iodes->seek);
	}

	return (rbytes);
}

ssize_t io_fread(t_iodes *iodes, char *buf, size_t nbytes)
{
	SSL_LOG(TRACE, "io fread with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);

	ssize_t	rbytes;

	if (NULL == buf || NULL == iodes) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}

	if (nbytes == 0) {
		SSL_LOG(TRACE, "buffer size is 0, nothing to read");
		return (0);
	}

	if (iodes->delim) {
		SSL_LOG(TRACE, "using delimiter-based read, delim=%d", iodes->delim);
		rbytes = __fread_delim(iodes, (char *)buf, nbytes);
	}
	else {
		SSL_LOG(TRACE, "using read without delimiter");
		rbytes = __fread(iodes, (char *)buf, nbytes);
	}

	return (rbytes);
}

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
