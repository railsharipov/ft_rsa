#include <io.h>

typedef struct s_io_v2_buffered_writer_ctx {
	t_io_v2_stream *downstream;
	t_buffer *buffer;
} t_io_v2_buffered_writer_ctx;

static ssize_t  __io_v2_buffered_stream_write(void *vctx, const void *buf, size_t nbytes);
static ssize_t  __io_v2_buffered_stream_flush(void *vctx);
static ssize_t  __io_v2_buffered_stream_close(void *vctx);
static ssize_t __io_v2_write_adapter(void *ctx, const void *buf, size_t nbytes);

int io_v2_buffered_writer(t_io_v2_stream **stream, t_io_v2_stream *downstream, size_t capacity)
{
    const t_io_v2_interface interface = {
        .write = __io_v2_buffered_stream_write,
        .flush = __io_v2_buffered_stream_flush,
        .close = __io_v2_buffered_stream_close,
    };
    t_io_v2_buffered_writer_ctx *ctx;

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (NULL == downstream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (capacity == 0) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_v2_buffered_writer_ctx));
    ctx->downstream = downstream;
    ctx->buffer = ft_buffer_new(capacity);

    SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    (*stream)->ctx = ctx;
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_WRITE | IO_V2_FLAG_FLUSH | IO_V2_FLAG_CLOSE;
    (*stream)->status = IO_V2_STATUS_OK;

    return (SSL_OK);
}

static ssize_t __io_v2_write_adapter(void *ctx, const void *buf, size_t nbytes)
{
	t_io_v2_stream *stream;

	stream = (t_io_v2_stream *)ctx;
	return (stream->interface.write(stream->ctx, buf, nbytes));
}

static ssize_t __io_v2_buffered_stream_write(void *vctx, const void *buf, size_t nbytes)
{
    t_io_v2_buffered_writer_ctx *ctx;
	t_io_v2_stream *stream;
	ssize_t wbytes;
	ssize_t rbytes;
	ssize_t tbytes;

	ctx = (t_io_v2_buffered_writer_ctx *)vctx;
	stream = ctx->downstream;
	tbytes = 0;

	while (tbytes < nbytes) {
		if (ft_buffer_is_full(ctx->buffer)) {
			IO_LOG(TRACE, "buffer is full, reading %zu bytes from buffer to stream", ft_buffer_used(ctx->buffer));
			rbytes = ft_buffer_read_with_func(ctx->buffer, __io_v2_write_adapter, stream, ft_buffer_used(ctx->buffer));
			if (rbytes < 0) {
				IO_LOG(ERROR, "failed to read from buffer to stream");
				return (IO_V2_STATUS_ERROR);
			}
			IO_LOG(TRACE, "read %zu bytes from buffer to stream", rbytes);
			if (stream->status == IO_V2_STATUS_ERROR) {
				IO_LOG(TRACE, "stream is in error state, stopping write");
				return (IO_V2_STATUS_ERROR);
			}
			if (rbytes == 0) {
				IO_LOG(TRACE, "no more bytes to write, stopping write");
				return (tbytes);
			}
		}
		else {
			IO_LOG(TRACE, "writing %zu bytes to buffer", nbytes - tbytes);
			wbytes = ft_buffer_write(ctx->buffer, buf + tbytes, nbytes - tbytes);
			if (wbytes < 0) {
				IO_LOG(ERROR, "failed to write to buffer");
				return (IO_V2_STATUS_ERROR);
			}
			IO_LOG(TRACE, "wrote %zu bytes to buffer", wbytes);
			tbytes += wbytes;
		}
	}
	return (tbytes);
}

static ssize_t __io_v2_buffered_stream_flush(void *vctx)
{
    t_io_v2_buffered_writer_ctx *ctx;
    t_io_v2_stream *downstream;
    t_buffer *buffer;
    ssize_t wbytes;

    ctx = (t_io_v2_buffered_writer_ctx *)vctx;

    downstream = ctx->downstream;
    buffer = ctx->buffer;

    IO_LOG(TRACE, "flushing buffered stream with downstream=%p, buffer=%p, capacity=%zu", downstream, buffer, buffer->capacity);

    wbytes = ft_buffer_read_with_func(buffer, __io_v2_write_adapter, downstream, ft_buffer_used(buffer));
    if (wbytes < 0) {
        IO_LOG(ERROR, "failed to flush buffered stream");
        return (IO_V2_STATUS_ERROR);
    }
    return (wbytes);
}

static ssize_t __io_v2_buffered_stream_close(void *vctx)
{
    t_io_v2_buffered_writer_ctx *ctx;
    ssize_t flush_result;

    ctx = (t_io_v2_buffered_writer_ctx *)vctx;

    IO_LOG(TRACE, "closing buffered stream");

    if (!ft_buffer_is_empty(ctx->buffer)) {
        IO_LOG(TRACE, "flushing remaining %zu bytes before close", ft_buffer_used(ctx->buffer));
        flush_result = __io_v2_buffered_stream_flush(vctx);
        if (flush_result < 0) {
            IO_LOG(ERROR, "failed to flush buffer before close");
            return (IO_V2_STATUS_ERROR);
        }
    }

    if (io_v2_close(ctx->downstream) < 0) {
        IO_LOG(ERROR, "failed to close underlying stream");
        return (IO_V2_STATUS_ERROR);
    }
    ctx->downstream = NULL;

    if (NULL != ctx->buffer) {
        ft_buffer_del(ctx->buffer);
        ctx->buffer = NULL;
    }
    SSL_FREE(ctx);
    IO_LOG(TRACE, "buffered stream closed");

    return (IO_V2_STATUS_OK);
}