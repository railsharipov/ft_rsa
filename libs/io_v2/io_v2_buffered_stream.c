#include <libft_v2.h>

static ssize_t  __io_v2_buffered_read(void *vctx, void *buf, size_t nbytes);
static ssize_t  __io_v2_buffered_write(void *vctx, const void *buf, size_t nbytes);
static ssize_t  __io_v2_buffered_flush(void *vctx);
static ssize_t  __io_v2_buffered_close(void *vctx);
static ssize_t	__io_v2_buffered_noop_close(void *vctx);

static uint8_t	__stdin_arr[64*1024];
static uint8_t	__stdout_arr[64*1024];
static uint8_t	__stderr_arr[64*1024];

static t_buffer __stdin_buffer = { .arr = __stdin_arr, .capacity = sizeof(__stdin_arr), };
static t_buffer __stdout_buffer = { .arr = __stdout_arr, .capacity = sizeof(__stdout_arr), };
static t_buffer __stderr_buffer = { .arr = __stderr_arr, .capacity = sizeof(__stderr_arr), };

static t_io_v2_buffered_ctx __buffered_stdin_ctx = { .stream = &io_v2_stdin_reader, .buffer = &__stdin_buffer };
static t_io_v2_buffered_ctx __buffered_stdout_ctx = { .stream = &io_v2_stdout_writer, .buffer = &__stdout_buffer };
static t_io_v2_buffered_ctx __buffered_stderr_ctx = { .stream = &io_v2_stderr_writer, .buffer = &__stderr_buffer };

t_io_v2_stream io_v2_buffered_stdin_reader = {
	.interface = (t_io_v2_interface){ .read = __io_v2_buffered_read, .close = __io_v2_buffered_noop_close, },
	.flags = IO_V2_FLAG_READ|IO_V2_FLAG_CLOSE,
	.status = IO_V2_STATUS_OK,
	.ctx = &__buffered_stdin_ctx,
};
t_io_v2_stream io_v2_buffered_stdout_writer = {
	.interface = (t_io_v2_interface){ .write = __io_v2_buffered_write, .flush = __io_v2_buffered_flush, .close = __io_v2_buffered_noop_close, },
	.flags = IO_V2_FLAG_WRITE|IO_V2_FLAG_FLUSH|IO_V2_FLAG_CLOSE,
	.status = IO_V2_STATUS_OK,
	.ctx = &__buffered_stdout_ctx,
};
t_io_v2_stream io_v2_buffered_stderr_writer = {
	.interface = (t_io_v2_interface){ .write = __io_v2_buffered_write, .flush = __io_v2_buffered_flush, .close = __io_v2_buffered_noop_close, },
	.flags = IO_V2_FLAG_WRITE|IO_V2_FLAG_FLUSH|IO_V2_FLAG_CLOSE,
	.status = IO_V2_STATUS_OK,
	.ctx = &__buffered_stderr_ctx,
};

static ssize_t  __read_from_upstream(void *ctx, void *buf, size_t nbytes);
static ssize_t	__write_to_downstream(void *ctx, const void *buf, size_t nbytes);

bool io_v2_buffered_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream, size_t capacity)
{
    const t_io_v2_interface interface = {
        .read = __io_v2_buffered_read,
        .close = __io_v2_buffered_close,
    };
    t_io_v2_buffered_ctx *ctx;

    if (NULL == stream) {
        IO_LOG(ERROR, IO_INPUT_ERROR);
        return false;
    }
    if (NULL == upstream) {
        IO_LOG(ERROR, IO_INPUT_ERROR);
        return false;
    }
    if (capacity == 0) {
        IO_LOG(ERROR, IO_INPUT_ERROR);
        return false;
    }
    LIBFT_ALLOC(ctx, sizeof(t_io_v2_buffered_ctx));
    ctx->stream = upstream;
    ctx->buffer = ft_buffer_new(capacity);

	if (!io_v2_stream(stream, interface, ctx)) {
		IO_LOG(ERROR, IO_CREATE_STREAM_ERROR);
		return false;
	}
    return true;
}

bool io_v2_buffered_writer(t_io_v2_stream **stream, t_io_v2_stream *downstream, size_t capacity)
{
    const t_io_v2_interface interface = {
        .write = __io_v2_buffered_write,
        .flush = __io_v2_buffered_flush,
        .close = __io_v2_buffered_close,
    };
    t_io_v2_buffered_ctx *ctx;

    if (NULL == stream) {
        IO_LOG(ERROR, IO_INPUT_ERROR);
        return false;
    }
    if (NULL == downstream) {
        IO_LOG(ERROR, IO_INPUT_ERROR);
        return false;
    }
    if (capacity == 0) {
        IO_LOG(ERROR, IO_INPUT_ERROR);
        return false;
    }
    LIBFT_ALLOC(ctx, sizeof(t_io_v2_buffered_ctx));
	ctx->stream = downstream;
    ctx->buffer = ft_buffer_new(capacity);

	if (!io_v2_stream(stream, interface, ctx)) {
		IO_LOG(ERROR, IO_CREATE_STREAM_ERROR);
		return false;
	}
    return true;
}

static ssize_t __read_from_upstream(void *vctx, void *buf, size_t bufsize)
{
	t_io_v2_stream *upstream = vctx;
	// Reads data from upstream into the internal buffer
	ssize_t ret = io_v2_read(upstream, buf, bufsize);
	if (ret < 0 && upstream->status == IO_V2_STATUS_EOF) {
		return (0);
	}
	return ret;
}

static ssize_t __write_to_downstream(void *vctx, const void *buf, size_t bufsize)
{
	t_io_v2_stream *downstream = vctx;
	return (io_v2_write(downstream, buf, bufsize));
}

static ssize_t	__io_v2_buffered_read(void *vctx, void *buf, size_t nbytes)
{
	t_io_v2_buffered_ctx *ctx;
	t_io_v2_stream *upstream;
	ssize_t rbytes;
	ssize_t wbytes;
	size_t tbytes;

	IO_LOG(TRACE, "reading %zu bytes from buffered stream", nbytes);

	ctx = (t_io_v2_buffered_ctx *)vctx;
	upstream = ctx->stream;
	tbytes = 0;

	while (tbytes < nbytes) {
		if (ft_buffer_is_empty(ctx->buffer)) {
			IO_LOG(TRACE, "buffer is empty");
			switch (upstream->status) {
				case IO_V2_STATUS_OK:
					break;
				case IO_V2_STATUS_EOF:
					IO_LOG(TRACE, "stream is at EOF, stopping read");
					if (tbytes == 0) {
						return (IO_V2_STATUS_EOF);
					}
					return (tbytes);
				case IO_V2_STATUS_ERROR:
					IO_LOG(ERROR, "stream is in error state");
					return (IO_V2_STATUS_ERROR);
				case IO_V2_STATUS_CLOSED:
					IO_LOG(ERROR, "stream is closed");
					return (IO_V2_STATUS_ERROR);
				case IO_V2_STATUS_FINISHED:
				default:
					IO_LOG(ERROR, "invalid stream status");
					return (IO_V2_STATUS_ERROR);
			}
			IO_LOG(TRACE, "filling internal buffer from stream");
			wbytes = ft_buffer_write_with_func(ctx->buffer, __read_from_upstream, upstream, ctx->buffer->capacity);
			if (wbytes < 0) {
				IO_LOG(ERROR, "failed to write to buffer from stream");
				return (IO_V2_STATUS_ERROR);
			} else {
				IO_LOG(TRACE, "wrote %zu bytes to buffer from stream", wbytes);
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

static ssize_t __io_v2_buffered_write(void *vctx, const void *buf, size_t nbytes)
{
    t_io_v2_buffered_ctx *ctx;
	t_io_v2_stream *downstream;
	ssize_t wbytes;
	ssize_t rbytes;
	size_t tbytes;

	IO_LOG(TRACE, "writing %zu bytes to buffered stream", nbytes);

	ctx = (t_io_v2_buffered_ctx *)vctx;
	downstream = ctx->stream;
	tbytes = 0;

	while (tbytes < nbytes) {
		if (ft_buffer_is_full(ctx->buffer)) {
			switch (downstream->status) {
				case IO_V2_STATUS_OK:
					break;
				case IO_V2_STATUS_ERROR:
					IO_LOG(ERROR, "stream is in error state");
					return (IO_V2_STATUS_ERROR);
				case IO_V2_STATUS_CLOSED:
					IO_LOG(ERROR, "stream is closed");
					return (IO_V2_STATUS_ERROR);
				case IO_V2_STATUS_FINISHED:
				case IO_V2_STATUS_EOF:
				default:
					IO_LOG(ERROR, "invalid stream status");
					return (IO_V2_STATUS_ERROR);
			}
			IO_LOG(TRACE, "buffer is full, reading %zd bytes from buffer to stream", ft_buffer_used(ctx->buffer));
			rbytes = ft_buffer_read_with_func(ctx->buffer, __write_to_downstream, downstream, ft_buffer_used(ctx->buffer));
			if (rbytes < 0) {
				IO_LOG(ERROR, "failed to read from buffer to stream");
				return (IO_V2_STATUS_ERROR);
			} else {
				IO_LOG(TRACE, "read %zd bytes from buffer to stream", rbytes);
			}
		}
		else {
			IO_LOG(TRACE, "writing %zu bytes to buffer", nbytes - tbytes);
			wbytes = ft_buffer_write(ctx->buffer, buf + tbytes, nbytes - tbytes);
			if (wbytes < 0) {
				IO_LOG(ERROR, "failed to write to buffer");
				return (IO_V2_STATUS_ERROR);
			}
			IO_LOG(TRACE, "wrote %zd bytes to buffer", wbytes);
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

    IO_LOG(TRACE, "flushing buffered stream with downstream=%p, buffer=%p, capacity=%zu", downstream, buffer, buffer->capacity);

    wbytes = ft_buffer_read_with_func(buffer, __write_to_downstream, downstream, ft_buffer_used(buffer));
    if (wbytes < 0) {
		IO_LOG(ERROR, "failed to flush buffered stream");
		switch (downstream->status) {
			case IO_V2_STATUS_ERROR:
				IO_LOG(ERROR, "stream is in error state");
				return (IO_V2_STATUS_ERROR);
			case IO_V2_STATUS_CLOSED:
				IO_LOG(ERROR, "stream is closed");
				return (IO_V2_STATUS_ERROR);
			case IO_V2_STATUS_FINISHED:
			case IO_V2_STATUS_EOF:
			case IO_V2_STATUS_OK:
			    UNREACHABLE("__io_v2_buffered_flush");
			default:
				IO_LOG(ERROR, "invalid stream status");
				return (IO_V2_STATUS_ERROR);
		}
    } else {
		IO_LOG(TRACE, "flushed %zd bytes to downstream", wbytes);
	}
    return (wbytes);
}

static ssize_t __io_v2_buffered_close(void *vctx)
{
    t_io_v2_buffered_ctx *ctx = vctx;
    int flush_ret = IO_V2_STATUS_OK;
    int close_ret = IO_V2_STATUS_OK;

    IO_LOG(TRACE, "closing buffered stream");

    flush_ret = __io_v2_buffered_flush(vctx);
    if (flush_ret < 0) {
        IO_LOG(ERROR, "failed to flush buffered stream on close");
    } else {
        IO_LOG(TRACE, "flushed buffered stream on close");
    }
    close_ret = io_v2_close(ctx->stream);
    if (close_ret < 0) {
        IO_LOG(ERROR, "failed to close upstream");
    } else {
        IO_LOG(TRACE, "closed upstream");
    }
    ctx->stream = NULL;
    ft_buffer_del(ctx->buffer);
    ctx->buffer = NULL;
    LIBFT_FREE(ctx);
    IO_LOG(TRACE, "buffered stream closed");

    if (flush_ret < 0 || close_ret < 0) {
    	return (IO_V2_STATUS_ERROR);
    } else {
   		return (IO_V2_STATUS_OK);
    }
}

static ssize_t __io_v2_buffered_noop_close(void *vctx)
{
	(void)vctx;
    IO_LOG(TRACE, "ignoring file stream close");
    return (IO_V2_STATUS_OK);
}
