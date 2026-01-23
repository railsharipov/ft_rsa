#include <io.h>

typedef struct s_io_v2_buffered_reader_ctx {
	t_io_v2_stream *upstream;
	t_buffer *buffer;
} t_io_v2_buffered_reader_ctx;

static ssize_t  __io_v2_buffered_stream_read(void *vctx, void *buf, size_t nbytes);
static ssize_t  __io_v2_buffered_stream_close(void *vctx);
static ssize_t  __io_v2_read_adapter(void *ctx, void *buf, size_t nbytes);

int io_v2_buffered_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream, size_t capacity)
{
    const t_io_v2_interface interface = {
        .read = __io_v2_buffered_stream_read,
        .close = __io_v2_buffered_stream_close,
    };
    t_io_v2_buffered_reader_ctx *ctx;

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (NULL == upstream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (capacity == 0) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_v2_buffered_reader_ctx));
    ctx->upstream = upstream;
    ctx->buffer = ft_buffer_new(capacity);

    SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    (*stream)->ctx = ctx;
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_READ | IO_V2_FLAG_CLOSE;
    (*stream)->status = IO_V2_STATUS_OK;

    return (SSL_OK);
}

static ssize_t __io_v2_read_adapter(void *ctx, void *buf, size_t nbytes)
{
	t_io_v2_stream *stream;

	stream = (t_io_v2_stream *)ctx;
	return (stream->interface.read(stream->ctx, buf, nbytes));
}

static ssize_t	__io_v2_buffered_stream_read(void *vctx, void *buf, size_t nbytes)
{
	t_io_v2_buffered_reader_ctx *ctx;
	t_io_v2_stream *stream;
	ssize_t rbytes;
	ssize_t wbytes;
	ssize_t tbytes;

	ctx = (t_io_v2_buffered_reader_ctx *)vctx;
	stream = ctx->upstream;
	tbytes = 0;

	while (tbytes < nbytes) {
		if (ft_buffer_is_empty(ctx->buffer)) {
			IO_LOG(TRACE, "buffer is empty, writing %zu bytes from stream", ctx->buffer->capacity);
			wbytes = ft_buffer_write_with_func(ctx->buffer, __io_v2_read_adapter, stream, ctx->buffer->capacity);
			if (wbytes < 0) {
				IO_LOG(ERROR, "failed to write to buffer from stream");
				return (IO_V2_STATUS_ERROR);
			}
			IO_LOG(TRACE, "wrote %zu bytes to buffer from stream", wbytes);
			if (wbytes == 0) {
				IO_LOG(TRACE, "stream is at EOF, stopping read");
				if (stream->status == IO_V2_STATUS_EOF && tbytes == 0) {
					return (IO_V2_STATUS_EOF);
				}
				return (tbytes);
			}
		}
		else {
			IO_LOG(TRACE, "reading %zu bytes from buffer", nbytes - tbytes);
			rbytes = ft_buffer_read(ctx->buffer, buf + tbytes, nbytes - tbytes);
			if (rbytes < 0) {
				IO_LOG(ERROR, "failed to read from buffer");
				return (IO_V2_STATUS_ERROR);
			}
			IO_LOG(TRACE, "read %zu bytes from buffer", rbytes);
			tbytes += rbytes;
		}
	}
	return (tbytes);
}

static ssize_t __io_v2_buffered_stream_close(void *vctx)
{
    t_io_v2_buffered_reader_ctx *ctx;

    ctx = (t_io_v2_buffered_reader_ctx *)vctx;

    IO_LOG(TRACE, "closing buffered stream");

    if (io_v2_close(ctx->upstream) < 0) {
        IO_LOG(ERROR, "failed to close underlying stream");
        return (IO_V2_STATUS_ERROR);
    }
    ctx->upstream = NULL;

    if (NULL != ctx->buffer) {
        ft_buffer_del(ctx->buffer);
        ctx->buffer = NULL;
    }
    SSL_FREE(ctx);
    IO_LOG(TRACE, "buffered stream closed");

    return (IO_V2_STATUS_OK);
}