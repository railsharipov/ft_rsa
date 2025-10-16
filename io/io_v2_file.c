#include <unistd.h>
#include <sys/fcntl.h>
#include <io.h>
#include <libft/std.h>
#include <libft/alloc.h>

typedef struct	s_io_file_ctx
{
	int			fd;
	ssize_t		seek;
	int			delim;
	int			lwidth;
}				t_io_file_ctx;

static ssize_t	__io_file_read(void *ctx, char *buf, size_t nbytes);
static ssize_t	__io_file_write(void *ctx, const char *buf, size_t nbytes);

static ssize_t __fread(t_io_file_ctx *ctx, char *buf, size_t nbytes);
static ssize_t __fread_delim(t_io_file_ctx *ctx, char *buf, size_t nbytes);
static ssize_t __fwrite(t_io_file_ctx *ctx, const char *buf, size_t nbytes);
static ssize_t __fwrite_delim(t_io_file_ctx *ctx, const char *buf, size_t nbytes);

int	io_v2_fopen(t_iodes_v2 *iodes, uint32_t flags, const char *filename)
{
	t_io_file_ctx	*ctx;
	int	fd;

	IO_LOG(TRACE, "io fopen with flags=0x%x, filename=%s", flags, filename ? filename : "");

	if (NULL == iodes) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}

	fd = -1;
	if (SSL_FLAG(IO_STDIN, flags))
		fd = STDIN_FILENO;
	else if (SSL_FLAG(IO_STDOUT, flags))
		fd = STDOUT_FILENO;
	else if (SSL_FLAG(IO_FILE, flags)) {
		if (NULL == filename) {
			IO_LOG(ERROR, "filename is not specified");
			return (-1);
		}
		IO_LOG(TRACE, "opening file '%s' with flags 0x%x", filename, flags);
		if (SSL_FLAG(IO_READ, flags)) {
			fd = open(filename, O_RDONLY, 0644);
		}
		else if (SSL_FLAG(IO_WRITE, flags)) {
			fd = open(filename, O_TRUNC|O_RDWR|O_CREAT, 0644);
		}
	}

	if (fd < 0) {
		IO_LOG(ERROR, "file open failed, fd=%d", fd);
		return (-1);
	}

	SSL_ALLOC(ctx, sizeof(t_io_file_ctx));
	ctx->fd = fd;

	ft_bzero(iodes, sizeof(t_iodes_v2));
	iodes->ctx = ctx;
	iodes->read_f = __io_file_read;
	iodes->write_f = __io_file_write;

	return (0);
}

void	io_v2_fclose(t_iodes_v2 *iodes)
{
	t_io_file_ctx *ctx;

	if (NULL == iodes) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return ;
	}
	ctx = (t_io_file_ctx *)iodes->ctx;

	if (NULL == ctx) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return ;
	}
	IO_LOG(TRACE, "io fclose with iodes=%p, fd=%d", iodes, ctx->fd);
	if (ctx->fd > 2) {
		IO_LOG(TRACE, "closing file descriptor %d", ctx->fd);
		close(ctx->fd);
		IO_LOG(TRACE, "file descriptor %d closed", ctx->fd);
	} else {
		IO_LOG(TRACE, "skipping close for file descriptor: %d", ctx->fd);
	}
	SSL_FREE(ctx);
	iodes->ctx = NULL;
}

static ssize_t __io_file_read(void *vctx, char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "io file read with vctx=%p, buf=%p, nbytes=%zu", vctx, buf, nbytes);
	t_io_file_ctx *ctx;

	ssize_t	rbytes;

	ctx = (t_io_file_ctx *)vctx;
	if (NULL == buf || NULL == ctx) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}

	if (nbytes == 0) {
		IO_LOG(TRACE, "buffer size is 0, nothing to read");
		return (0);
	}
	if (ctx->delim) {
		IO_LOG(TRACE, "using delimiter-based read, delim=%d", ctx->delim);
		rbytes = __fread_delim(ctx, (char *)buf, nbytes);
	}
	else {
		IO_LOG(TRACE, "using read without delimiter");
		rbytes = __fread(ctx, (char *)buf, nbytes);
	}
	return (rbytes);
}

static ssize_t __fread(t_io_file_ctx *ctx, char *buf, size_t nbytes)
{
	ssize_t	rbytes;

	rbytes = read(ctx->fd, buf, nbytes);

	if (rbytes < 0) {
		IO_LOG(ERROR, "read error: %s (errno=%d)", strerror(errno), errno);
		return (-1);
	}
	IO_LOG(TRACE, "read %zd bytes from fd %d", rbytes, ctx->fd);

	if (rbytes > 0) {
		ctx->seek += rbytes;
		IO_LOG(TRACE, "updated seek to %zd", ctx->seek);
	}

	return (rbytes);
}

static ssize_t __fread_delim(t_io_file_ctx *ctx, char *buf, size_t nbytes)
{
	uint32_t	delim;
	ssize_t		rbytes;
	ssize_t		tbytes;
	char		tbuf[nbytes];
	char 		*tbufptr;

	delim = (uint32_t)ctx->delim;

	tbytes = 0;
	while (tbytes < nbytes) {
		tbufptr = tbuf;
		rbytes = read(ctx->fd, tbuf, nbytes-tbytes);
		IO_LOG(TRACE, "read %zd bytes from fd %d", rbytes, ctx->fd);

		if (rbytes < 0) {
			int err = errno;
			IO_LOG(ERROR, "read error: %s (errno=%d)", strerror(err), err);
			break ;
		}
		if (rbytes == 0) {
			IO_LOG(TRACE, "no bytes read from buffer, stop reading");
			break ;
		}

		ctx->seek += rbytes;
		IO_LOG(TRACE, "updated seek to %zd", ctx->seek);

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

static ssize_t	__io_file_write(void *vctx, const char *buf, size_t nbytes)
{
	ssize_t	wbytes;
	t_io_file_ctx *ctx;

	IO_LOG(TRACE, "io file write with vctx=%p, buf=%p, nbytes=%zu", vctx, buf, nbytes);

	ctx = (t_io_file_ctx *)vctx;
	if (NULL == buf || NULL == ctx) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}

	if (nbytes == 0) {
		IO_LOG(TRACE, "buffer size is 0, nothing to write");
		return (0);
	}
	if (ctx->delim) {
		IO_LOG(TRACE, "using delimiter-based write, delim=%d", ctx->delim);
		wbytes = __fwrite_delim(ctx, (char *)buf, nbytes);
	}
	else {
		IO_LOG(TRACE, "using write without delimiter");
		wbytes = __fwrite(ctx, (char *)buf, nbytes);
	}
	return (wbytes);
}

static ssize_t __fwrite(t_io_file_ctx *ctx, const char *buf, size_t nbytes)
{
	ssize_t	wbytes;

	IO_LOG(TRACE, "writing %zu bytes to fd %d", nbytes, ctx->fd);
	wbytes = write(ctx->fd, buf, nbytes);

	if (wbytes < 0) {
		IO_LOG(ERROR, "write error: %s (errno=%d)", strerror(errno), errno);
		return (-1);
	}
	IO_LOG(TRACE, "write %zd bytes to fd %d", wbytes, ctx->fd);

	if (wbytes > 0) {
		ctx->seek += wbytes;
		IO_LOG(TRACE, "updated seek to %zd", ctx->seek);
	}

	return (wbytes);
}

static ssize_t	__fwrite_delim(t_io_file_ctx *ctx, const char *buf, size_t nbytes)
{
	char	tbuf[2*nbytes];
	ssize_t	wbytes;
	ssize_t	offset;
	ssize_t	width;
	ssize_t	ix;

	if (ctx->lwidth <= 0) {
		IO_LOG(TRACE, "setting default lwidth to 64");
		ctx->lwidth = 64;
	}

	width = (size_t)ctx->lwidth;
	offset = MIN(nbytes, (ctx->seek % width));
	wbytes = 0;

	IO_LOG(TRACE, "width=%zd, offset=%zd", width, offset);

	/* Finish last line if it's not [width] bytes long */
	if (offset) {
		IO_LOG(TRACE, "finishing last line with offset");
		for (; wbytes < (width - offset); nbytes--)
			tbuf[wbytes++] = *buf++;
		if (nbytes > 0) {
			tbuf[wbytes++] = ctx->delim;
		}
	}

	while (nbytes/width > 0) {
		IO_LOG(TRACE, "writing full width line");
		for (ix = 0; ix < width; ix++, nbytes--) {
			tbuf[wbytes++] = *buf++;
		}
		tbuf[wbytes++] = ctx->delim;
	}

	while (nbytes-- > 0)
		tbuf[wbytes++] = *buf++;

	IO_LOG(TRACE, "writing %zd bytes to fd %d", wbytes, ctx->fd);
	wbytes = write(ctx->fd, tbuf, wbytes);

	if (wbytes < 0) {
		IO_LOG(ERROR, "write error: %s (errno=%d)", strerror(errno), errno);
		return (-1);
	}
	IO_LOG(TRACE, "write %zd bytes to fd %d", wbytes, ctx->fd);

	if (wbytes > 0) {
		ctx->seek += wbytes;
		IO_LOG(TRACE, "updated seek to %zd", ctx->seek);
	}

	return (wbytes);
}
