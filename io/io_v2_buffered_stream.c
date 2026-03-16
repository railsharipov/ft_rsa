#include <io.h>

typedef struct s_io_v2_buffered_ctx {
	t_io_v2_stream *stream;
	t_buffer *buffer;
} t_io_v2_buffered_ctx;

static ssize_t  __io_v2_buffered_read(void *vctx, void *buf, size_t nbytes);
static ssize_t  __io_v2_buffered_write(void *vctx, const void *buf, size_t nbytes);
static ssize_t  __io_v2_buffered_flush(void *vctx);
static ssize_t  __io_v2_buffered_close(void *vctx);

static ssize_t  __read_from_upstream(void *ctx, void *buf, size_t nbytes);
static ssize_t	__write_to_downstream(void *ctx, const void *buf, size_t nbytes);

int io_v2_buffered_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream, size_t capacity)
{
    const t_io_v2_interface interface = {
        .read = __io_v2_buffered_read,
        .close = __io_v2_buffered_close,
    };
    t_io_v2_buffered_ctx *ctx;

    if (NULL == stream) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (NULL == upstream) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (capacity == 0) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_v2_buffered_ctx));
    ctx->stream = upstream;
    ctx->buffer = ft_buffer_new(capacity);

	if (SSL_OK != io_v2_stream(stream, interface, ctx)) {
		SSL_LOG(ERROR, IO_CREATE_STREAM_ERROR);
		return (SSL_ERR);
	}
    return (SSL_OK);
}

int io_v2_buffered_writer(t_io_v2_stream **stream, t_io_v2_stream *downstream, size_t capacity)
{
    const t_io_v2_interface interface = {
        .write = __io_v2_buffered_write,
        .flush = __io_v2_buffered_flush,
        .close = __io_v2_buffered_close,
    };
    t_io_v2_buffered_ctx *ctx;

    if (NULL == stream) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (NULL == downstream) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (capacity == 0) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_v2_buffered_ctx));
	ctx->stream = downstream;
    ctx->buffer = ft_buffer_new(capacity);

	if (SSL_OK != io_v2_stream(stream, interface, ctx)) {
		SSL_LOG(ERROR, IO_CREATE_STREAM_ERROR);
		return (SSL_ERR);
	}
    return (SSL_OK);
}

static ssize_t __read_from_upstream(void *ctx, void *buf, size_t nbytes)
{
	t_io_v2_stream *upstream;

	upstream = (t_io_v2_stream *)ctx;
	return (io_v2_read(upstream, buf, nbytes));
}

static ssize_t __write_to_downstream(void *ctx, const void *buf, size_t nbytes)
{
	t_io_v2_stream *downstream;

	downstream = (t_io_v2_stream *)ctx;
	return (io_v2_write(downstream, buf, nbytes));
}

static ssize_t	__io_v2_buffered_read(void *vctx, void *buf, size_t nbytes)
{
	t_io_v2_buffered_ctx *ctx;
	t_io_v2_stream *upstream;
	ssize_t rbytes;
	ssize_t wbytes;
	size_t tbytes;

	SSL_LOG(TRACE, "reading %zu bytes from buffered stream", nbytes);

	ctx = (t_io_v2_buffered_ctx *)vctx;
	upstream = ctx->stream;
	tbytes = 0;

	while (tbytes < nbytes) {
		if (ft_buffer_is_empty(ctx->buffer)) {
			switch (upstream->status) {
				case IO_V2_STATUS_OK:
					break;
				case IO_V2_STATUS_EOF:
					SSL_LOG(TRACE, "stream is at EOF, stopping read");
					if (tbytes == 0) {
						return (IO_V2_STATUS_EOF);
					}
					return (tbytes);
				case IO_V2_STATUS_ERROR:
					SSL_LOG(ERROR, "stream is in error state");
					return (IO_V2_STATUS_ERROR);
				case IO_V2_STATUS_CLOSED:
					SSL_LOG(ERROR, "stream is closed");
					return (IO_V2_STATUS_ERROR);
				default:
					SSL_LOG(ERROR, "invalid stream status");
					return (IO_V2_STATUS_ERROR);
			}
			SSL_LOG(TRACE, "buffer is empty, writing %zu bytes to buffer from stream", ctx->buffer->capacity);
			wbytes = ft_buffer_write_with_func(ctx->buffer, __read_from_upstream, upstream, ctx->buffer->capacity);
			if (wbytes < 0) {
				SSL_LOG(ERROR, "failed to write to buffer from stream");
			} else {
				SSL_LOG(TRACE, "wrote %zu bytes to buffer from stream", wbytes);
			}
		}
		else {
			SSL_LOG(TRACE, "reading %zu bytes from buffer", nbytes - tbytes);
			rbytes = ft_buffer_read(ctx->buffer, buf + tbytes, nbytes - tbytes);
			if (rbytes < 0) {
				SSL_LOG(ERROR, "failed to read from buffer");
				return (IO_V2_STATUS_ERROR);
			}
			SSL_LOG(TRACE, "read %zu bytes from buffer", rbytes);
			tbytes += rbytes;
		}
	}
	return (tbytes);
}

static ssize_t __io_v2_buffered_write(void *vctx, const void *buf, size_t nbytes)
{
    t_io_v2_buffered_ctx *ctx;
	t_io_v2_stream *downstream;
	ssize_t wbytes;
	ssize_t rbytes;
	size_t tbytes;

	SSL_LOG(TRACE, "writing %zu bytes to buffered stream", nbytes);

	ctx = (t_io_v2_buffered_ctx *)vctx;
	downstream = ctx->stream;
	tbytes = 0;

	while (tbytes < nbytes) {
		if (ft_buffer_is_full(ctx->buffer)) {
			switch (downstream->status) {
				case IO_V2_STATUS_OK:
					break;
				case IO_V2_STATUS_ERROR:
					SSL_LOG(ERROR, "stream is in error state");
					return (IO_V2_STATUS_ERROR);
				case IO_V2_STATUS_CLOSED:
					SSL_LOG(ERROR, "stream is closed");
					return (IO_V2_STATUS_ERROR);
				default:
					SSL_LOG(ERROR, "invalid stream status");
					return (IO_V2_STATUS_ERROR);
			}
			SSL_LOG(TRACE, "buffer is full, reading %zd bytes from buffer to stream", ft_buffer_used(ctx->buffer));
			rbytes = ft_buffer_read_with_func(ctx->buffer, __write_to_downstream, downstream, ft_buffer_used(ctx->buffer));
			if (rbytes < 0) {
				SSL_LOG(ERROR, "failed to read from buffer to stream");
			} else {
				SSL_LOG(TRACE, "read %zd bytes from buffer to stream", rbytes);
			}
		}
		else {
			SSL_LOG(TRACE, "writing %zu bytes to buffer", nbytes - tbytes);
			wbytes = ft_buffer_write(ctx->buffer, buf + tbytes, nbytes - tbytes);
			if (wbytes < 0) {
				SSL_LOG(ERROR, "failed to write to buffer");
				return (IO_V2_STATUS_ERROR);
			}
			SSL_LOG(TRACE, "wrote %zd bytes to buffer", wbytes);
			tbytes += wbytes;
		}
	}
	return (tbytes);
}

static ssize_t __io_v2_buffered_flush(void *vctx)
{
    t_io_v2_buffered_ctx *ctx;
    t_io_v2_stream *downstream;
    t_buffer *buffer;
    size_t wbytes;

    ctx = (t_io_v2_buffered_ctx *)vctx;

    downstream = ctx->stream;
    buffer = ctx->buffer;

    SSL_LOG(TRACE, "flushing buffered stream with downstream=%p, buffer=%p, capacity=%zu", downstream, buffer, buffer->capacity);

    wbytes = ft_buffer_read_with_func(buffer, __write_to_downstream, downstream, ft_buffer_used(buffer));
    if (wbytes < 0) {
		SSL_LOG(ERROR, "failed to flush buffered stream");
		switch (downstream->status) {
			case IO_V2_STATUS_ERROR:
				SSL_LOG(ERROR, "stream is in error state");
				return (IO_V2_STATUS_ERROR);
			case IO_V2_STATUS_CLOSED:
				SSL_LOG(ERROR, "stream is closed");
				return (IO_V2_STATUS_ERROR);
			default:
				SSL_LOG(ERROR, "invalid stream status");
				return (IO_V2_STATUS_ERROR);
		}
    } else {
		SSL_LOG(TRACE, "flushed %zd bytes to downstream", wbytes);
	}
    return (wbytes);
}

static ssize_t __io_v2_buffered_close(void *vctx)
{
    t_io_v2_buffered_ctx *ctx;

    ctx = (t_io_v2_buffered_ctx *)vctx;

    SSL_LOG(TRACE, "closing buffered stream");

    if (io_v2_close(ctx->stream) < 0) {
        SSL_LOG(ERROR, "failed to close upstream");
        return (IO_V2_STATUS_ERROR);
    } else {
        SSL_LOG(TRACE, "closed upstream");
    }
    ctx->stream = NULL;

    if (NULL != ctx->buffer) {
        ft_buffer_del(ctx->buffer);
        ctx->buffer = NULL;
    }
    SSL_FREE(ctx);
    SSL_LOG(TRACE, "buffered stream closed");

    return (IO_V2_STATUS_OK);
}
