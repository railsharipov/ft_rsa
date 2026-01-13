#include <unistd.h>
#include <sys/fcntl.h>
#include <io.h>
#include <file.h>
#include <libft/std.h>
#include <libft/alloc.h>

typedef struct s_file_stream_ctx {
	t_io_v2_stream	*file_stream;
	t_file			*file;
} t_file_stream_ctx;

static ssize_t	__io_file_read(t_file_stream_ctx *ctx, char *buf, size_t nbytes);
static ssize_t	__io_file_write(t_file_stream_ctx *ctx, char *buf, size_t nbytes);
static void		__io_file_close(t_file_stream_ctx *ctx);

int	file_reader(t_io_v2_stream **stream, t_file *file)
{
	return (file_stream(stream, file, IO_V2_FLAG_READ));
}

int	file_writer(t_io_v2_stream **stream, t_file *file)
{
	return (file_stream(stream, file, IO_V2_FLAG_WRITE));
}

int	file_stream(t_io_v2_stream **stream, t_file *file, t_io_v2_flag flags)
{
	const t_io_v2_interface	io_interface = {
		.read = __io_file_read,
		.write = __io_file_write,
		.close = __io_file_close,
	};
	t_file_stream_ctx *ctx;

	FILE_LOG(TRACE, "file stream with flags=%#x, file=%p", flags, file);

	if (NULL == stream) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == file) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_ALLOC(*stream, sizeof(t_io_v2_stream));

	if (NULL == file->path) {
		FILE_LOG(ERROR, "file path is not specified");
		return (SSL_ERR);
	}
	FILE_LOG(TRACE, "opening file '%s'", file->path);
	file->fd = open(file->path, O_TRUNC|O_RDWR|O_CREAT, 0644);

	if (file->fd < 0) {
		FILE_LOG(ERROR, "file open failed: %s", strerror(errno));
		return (SSL_ERR);
	}
	SSL_ALLOC(ctx, sizeof(t_file_stream_ctx));
	ctx->file = file;
	ctx->file_stream = *stream;

	return (io_v2_stream(stream, ctx, io_interface, flags | IO_V2_FLAG_CLOSE));
}

static ssize_t __io_file_read(t_file_stream_ctx *ctx, char *buf, size_t nbytes)
{
	ssize_t	rbytes;
	int		err;

	FILE_LOG(TRACE, "io file read with file=%p, buf=%p, nbytes=%zu", ctx->file, buf, nbytes);

	if (nbytes == 0) {
		FILE_LOG(TRACE, "buffer size is 0, nothing to read");
		return (0);
	}
	rbytes = read(ctx->file->fd, buf, nbytes);

	if (rbytes < 0) {
		err = errno;
		FILE_LOG(ERROR, "read error: %s (errno=%d)", strerror(err), err);
		return (-1);
	}
	FILE_LOG(TRACE, "read %zd bytes from fd %d", rbytes, ctx->file->fd);

	if (rbytes == 0) {
		FILE_LOG(TRACE, "no bytes read, stream is at EOF");
		ctx->file_stream->status = IO_V2_STATUS_EOF;
	}
	ctx->file->seek += rbytes;
	FILE_LOG(TRACE, "updated seek to %zd", ctx->file->seek);

	return (rbytes);
}

static ssize_t	__io_file_write(t_file_stream_ctx *ctx, char *buf, size_t nbytes)
{
	ssize_t	wbytes;
	int		err;

	FILE_LOG(TRACE, "io file write with file=%p, buf=%p, nbytes=%zu", ctx->file, buf, nbytes);

	if (nbytes == 0) {
		FILE_LOG(TRACE, "buffer size is 0, nothing to write");
		return (0);
	}
	wbytes = write(ctx->file->fd, buf, nbytes);

	if (wbytes < 0) {
		err = errno;
		FILE_LOG(ERROR, "write error: %s (errno=%d)", strerror(err), err);
		return (-1);
	}
	FILE_LOG(TRACE, "write %zd bytes to fd %d", wbytes, ctx->file->fd);

	if (wbytes == 0) {
		FILE_LOG(TRACE, "no bytes written, stream is at EOF");
		ctx->file_stream->status = IO_V2_STATUS_EOF;
	}
	ctx->file->seek += wbytes;
	FILE_LOG(TRACE, "updated seek to %zd", ctx->file->seek);

	return (wbytes);
}

static void	__io_file_close(t_file_stream_ctx *ctx)
{
	if (NULL == ctx) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return;
	}
	if (ctx->file->fd > 2) {
		FILE_LOG(TRACE, "closing file descriptor %d", ctx->file->fd);
		close(ctx->file->fd);
		FILE_LOG(TRACE, "file descriptor %d closed", ctx->file->fd);
	}
	else {
		FILE_LOG(TRACE, "skipping close for file descriptor: %d", ctx->file->fd);
	}
	ctx->file_stream->status = IO_V2_STATUS_CLOSED;
	SSL_FREE(ctx);
}