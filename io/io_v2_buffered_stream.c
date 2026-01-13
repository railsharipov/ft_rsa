#include <io.h>
#include <libft/std.h>
#include <libft/alloc.h>
#include <libft/buffer.h>

typedef struct s_io_v2_buffered_stream_ctx {
	t_io_v2_stream	*buffered_stream;
	t_io_v2_stream	*stream;
	t_buffer		*buffer;
} t_io_v2_buffered_stream_ctx;

static ssize_t	__io_v2_buffered_stream_read(t_io_v2_buffered_stream_ctx *ctx, char *buf, size_t nbytes);
static ssize_t	__io_v2_buffered_stream_write(t_io_v2_buffered_stream_ctx *ctx, char *buf, size_t nbytes);
static ssize_t	__io_v2_buffered_stream_flush(t_io_v2_buffered_stream_ctx *ctx);
static void		__io_v2_buffered_stream_close(t_io_v2_buffered_stream_ctx *ctx);

int	io_v2_buffered_stream(t_io_v2_stream **buffered_stream, t_io_v2_stream *stream, size_t capacity)
{
	const t_io_v2_interface	io_interface = {
		.read = __io_v2_buffered_stream_read,
		.write = __io_v2_buffered_stream_write,
		.flush = __io_v2_buffered_stream_flush,
		.close = __io_v2_buffered_stream_close,
	};
	t_io_v2_buffered_stream_ctx *ctx;
	t_buffer *buffer;

	IO_LOG(TRACE, "io buffered stream with stream=%p, capacity=%zu", stream, capacity);

	if (NULL == buffered_stream) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == stream) {
		IO_LOG(ERROR, "stream is not specified");
		return (SSL_ERR);
	}
	if (capacity == 0) {
		IO_LOG(ERROR, "capacity is not specified");
		return (SSL_ERR);
	}
	buffer = ft_buffer_new(capacity);
	if (NULL == buffer) {
		IO_LOG(ERROR, "failed to create buffer");
		return (SSL_ERR);
	}
	SSL_ALLOC(*buffered_stream, sizeof(t_io_v2_stream));
	SSL_ALLOC(ctx, sizeof(t_io_v2_buffered_stream_ctx));
	ctx->buffer = buffer;
	ctx->stream = stream;
	ctx->buffered_stream = *buffered_stream;
	(*buffered_stream)->ctx = ctx;
	(*buffered_stream)->interface = io_interface;
	(*buffered_stream)->type = IO_V2_TYPE_BUFFERED;
	(*buffered_stream)->flags = stream->flags | IO_V2_FLAG_FLUSH | IO_V2_FLAG_CLOSE;
	(*buffered_stream)->status = IO_V2_STATUS_OK;
	return (SSL_OK);
}

static ssize_t	__io_v2_buffered_stream_read(t_io_v2_buffered_stream_ctx *ctx, char *buf, size_t nbytes)
{
	t_io_v2_stream *stream;
	ssize_t rbytes;
	ssize_t wbytes;
	ssize_t tbytes;

	stream = ctx->stream;
	tbytes = 0;

	while (tbytes < nbytes) {
		if (ft_buffer_is_empty(ctx->buffer)) {
			IO_LOG(TRACE, "buffer is empty, writing %zu bytes from stream", ctx->buffer->capacity);
			wbytes = ft_buffer_write_with_func(ctx->buffer, stream->interface.read, stream->ctx, ctx->buffer->capacity);
			if (wbytes < 0) {
				IO_LOG(ERROR, "failed to write to buffer from stream");
				return (-1);
			}
			IO_LOG(TRACE, "wrote %zu bytes to buffer from stream", wbytes);
			if (wbytes == 0 && stream->status == IO_V2_STATUS_EOF) {
				IO_LOG(TRACE, "stream is at EOF, stopping read");
				ctx->buffered_stream->status = IO_V2_STATUS_EOF;
				break;
			}
		}
		else {
			IO_LOG(TRACE, "reading %zu bytes from buffer", nbytes - tbytes);
			rbytes = ft_buffer_read(ctx->buffer, buf + tbytes, nbytes - tbytes);
			if (rbytes < 0) {
				IO_LOG(ERROR, "failed to read from buffer");
				return (-1);
			}
			IO_LOG(TRACE, "read %zu bytes from buffer", rbytes);
			tbytes += rbytes;
		}
	}
	return (tbytes);
}

static ssize_t	__io_v2_buffered_stream_write(t_io_v2_buffered_stream_ctx *ctx, char *buf, size_t nbytes)
{
	t_io_v2_stream *stream;
	ssize_t wbytes;
	ssize_t rbytes;
	ssize_t tbytes;

	stream = ctx->stream;
	tbytes = 0;

	while (tbytes < nbytes) {
		if (ft_buffer_is_full(ctx->buffer)) {
			IO_LOG(TRACE, "buffer is full, reading %zu bytes from buffer to stream", ft_buffer_used(ctx->buffer));
			rbytes = ft_buffer_read_with_func(ctx->buffer, stream->interface.write, stream->ctx, ft_buffer_used(ctx->buffer));
			if (rbytes < 0) {
				IO_LOG(ERROR, "failed to read from buffer to stream");
				return (-1);
			}
			IO_LOG(TRACE, "read %zu bytes from buffer to stream", rbytes);
			if (ctx->stream->status == IO_V2_STATUS_EOF) {
				IO_LOG(TRACE, "stream is full, stopping write");
				ctx->buffered_stream->status = IO_V2_STATUS_EOF;
				break;
			}
		}
		else {
			IO_LOG(TRACE, "writing %zu bytes to buffer", nbytes - tbytes);
			wbytes = ft_buffer_write(ctx->buffer, buf + tbytes, nbytes - tbytes);
			if (wbytes < 0) {
				IO_LOG(ERROR, "failed to write to buffer");
				return (-1);
			}
			IO_LOG(TRACE, "wrote %zu bytes to buffer", wbytes);
			tbytes += wbytes;
		}
	}
	return (tbytes);
}

static ssize_t	__io_v2_buffered_stream_flush(t_io_v2_buffered_stream_ctx *ctx)
{
	t_io_v2_stream *stream;
	ssize_t rbytes;
	ssize_t tbytes;

	if (ft_buffer_is_empty(ctx->buffer)) {
		IO_LOG(TRACE, "buffer is empty, nothing to flush");
		return (0);
	}
	stream = ctx->stream;

	while (!ft_buffer_is_empty(ctx->buffer)) {
		IO_LOG(TRACE, "flushing %zu bytes from buffer to stream", ft_buffer_used(ctx->buffer));
		rbytes = ft_buffer_read_with_func(ctx->buffer, stream->interface.write, stream->ctx, ft_buffer_used(ctx->buffer));
		if (rbytes < 0) {
			IO_LOG(ERROR, "failed to flush from buffer to stream");
			return (-1);
		}
		if (ctx->stream->status == IO_V2_STATUS_EOF) {
			IO_LOG(TRACE, "stream is full, stopping write");
			ctx->buffered_stream->status = IO_V2_STATUS_EOF;
			break;
		}
		IO_LOG(TRACE, "flushed %zu bytes from buffer to stream", rbytes);
		tbytes += rbytes;
	}
	return (tbytes);
}

static void	__io_v2_buffered_stream_close(t_io_v2_buffered_stream_ctx *ctx)
{
	io_v2_close(ctx->stream);
	ft_buffer_del(ctx->buffer);
	SSL_FREE(ctx);
}