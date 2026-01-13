#include <io.h>
#include <bytes.h>
#include <libft/std.h>
#include <libft/alloc.h>

typedef struct s_bytes_stream_ctx {
	t_io_v2_stream	*bytes_stream;
	t_bytes			*bytes;
} t_bytes_stream_ctx;

static ssize_t	__io_bytes_read(t_bytes_stream_ctx *ctx, char *buf, size_t nbytes);
static ssize_t	__io_bytes_write(t_bytes_stream_ctx *ctx, const char *buf, size_t nbytes);

int	bytes_reader(t_io_v2_stream **stream, t_bytes *bytes)
{
	return (bytes_stream(stream, bytes, IO_V2_FLAG_READ));
}

int	bytes_writer(t_io_v2_stream **stream, t_bytes *bytes)
{
	return (bytes_stream(stream, bytes, IO_V2_FLAG_WRITE));
}

int	bytes_stream(t_io_v2_stream **stream, t_bytes *bytes, t_io_v2_flag flags)
{
	const t_io_v2_interface	io_interface = {
		.read = __io_bytes_read,
		.write = __io_bytes_write,
	};
	t_bytes_stream_ctx *ctx;

	BYTES_LOG(TRACE, "bytes stream with flags=%#x, bytes=%p", flags, bytes);

	if (NULL == stream) {
		BYTES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == bytes) {
		BYTES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == bytes->ostring) {
		BYTES_LOG(ERROR, "octet string is not specified");
		return (SSL_ERR);
	}
	BYTES_LOG(TRACE, "initializing bytes stream with ostring=%p", bytes->ostring);
	SSL_ALLOC(ctx, sizeof(t_bytes_stream_ctx));
	ctx->bytes = bytes;
	ctx->bytes_stream = *stream;

	return (io_v2_stream(stream, ctx, io_interface, flags | IO_V2_FLAG_CLOSE));
}

static ssize_t __io_bytes_read(t_bytes_stream_ctx *ctx, char *buf, size_t nbytes)
{
	size_t	rbytes;
	t_bytes *bytes;

	BYTES_LOG(TRACE, "io bytes read with bytes=%p, buf=%p, nbytes=%zu", ctx->bytes, buf, nbytes);
	bytes = ctx->bytes;

	if (nbytes == 0) {
		BYTES_LOG(TRACE, "buffer size is 0, nothing to read");
		return (0);
	}
	nbytes = MIN(nbytes, MAX(0, bytes->ostring->size - bytes->seek));
	BYTES_LOG(TRACE, "reading %zu bytes, ostring size=%zu, seek=%zd", nbytes, bytes->ostring->size, bytes->seek);

	rbytes = 0;
	while (nbytes-- > 0) {
		buf[rbytes++] = bytes->ostring->content[bytes->seek++];
	}
	BYTES_LOG(TRACE, "read %zd bytes", rbytes);

	if (bytes->seek >= bytes->ostring->size) {
		BYTES_LOG(TRACE, "reached end of ostring, stream is at EOF");
		ctx->bytes_stream->status = IO_V2_STATUS_EOF;
	}
	return (rbytes);
}

static ssize_t	__io_bytes_write(t_bytes_stream_ctx *ctx, const char *buf, size_t nbytes)
{
	BYTES_LOG(TRACE, "io bytes write with bytes=%p, buf=%p, nbytes=%zu", ctx->bytes, buf, nbytes);
	t_bytes *bytes;

	bytes = ctx->bytes;
	if (nbytes == 0) {
		BYTES_LOG(TRACE, "buffer size is 0, nothing to write");
		return (0);
	}

	BYTES_LOG(TRACE, "writing %zd bytes to ostring, current ostring size=%zu, seek=%zd", nbytes, bytes->ostring->size, bytes->seek);
	ft_ostr_append(bytes->ostring, (char *)buf, nbytes);
	bytes->seek += nbytes;
	BYTES_LOG(TRACE, "wrote %zd bytes, new ostring size=%zu, seek=%zd", nbytes, bytes->ostring->size, bytes->seek);

	return (nbytes);
}
