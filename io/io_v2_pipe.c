#include <io.h>

static ssize_t __io_v2_read_adapter(void *ctx, void *buf, size_t nbytes);
static ssize_t __io_v2_write_adapter(void *ctx, const void *buf, size_t nbytes);

typedef struct s_io_v2_pipe_ctx {
	t_io_v2_stream *upstream;
	t_io_v2_stream *downstream;
	t_buffer *buffer;
} t_io_v2_pipe_ctx;

int io_v2_pipe_unidir(t_io_v2_pipe **pipe, t_io_v2_stream *upstream, t_io_v2_stream *downstream, size_t capacity)
{
	t_io_v2_pipe_ctx *ctx;

    if (NULL == pipe || NULL == upstream || NULL == downstream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
	if (capacity == 0) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
    if (!SSL_FLAG(IO_V2_FLAG_READ, upstream->flags)) {
        IO_LOG(ERROR, "upstream stream is not readable");
        return (SSL_ERR);
    }
    if (!SSL_FLAG(IO_V2_FLAG_WRITE, downstream->flags)) {
        IO_LOG(ERROR, "downstream stream is not writable");
        return (SSL_ERR);
    }
    SSL_ALLOC((*pipe), sizeof(t_io_v2_pipe));
	SSL_ALLOC(ctx, sizeof(t_io_v2_pipe_ctx));
	ctx->upstream = upstream;
	ctx->downstream = downstream;
	ctx->buffer = ft_buffer_new(capacity);
    (*pipe)->ctx = ctx;
    (*pipe)->type = IO_V2_PIPE_TYPE_UNIDIR;
    (*pipe)->status = IO_V2_STATUS_OK;
    return (SSL_OK);
}

int io_v2_pipe_bidir(t_io_v2_pipe **pipe, t_io_v2_stream *upstream, t_io_v2_stream *downstream, size_t capacity)
{
	if (NULL == pipe || NULL == upstream || NULL == downstream) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (capacity == 0) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
    IO_LOG(ERROR, NOT_IMPLEMENTED_ERROR);

    return (SSL_ERR);
}

static ssize_t __io_v2_read_adapter(void *ctx, void *buf, size_t nbytes)
{
	t_io_v2_stream *upstream;

	upstream = (t_io_v2_stream *)ctx;
	return (io_v2_read(upstream, buf, nbytes));
}

static ssize_t __io_v2_write_adapter(void *ctx, const void *buf, size_t nbytes)
{
	t_io_v2_stream *downstream;

	downstream = (t_io_v2_stream *)ctx;
	return (io_v2_write(downstream, buf, nbytes));
}

ssize_t io_v2_pipe_pump(t_io_v2_pipe *pipe, size_t nbytes)
{
	t_io_v2_pipe_ctx *ctx;
    t_io_v2_stream *upstream, *downstream;
    ssize_t rbytes, wbytes;
    t_buffer *buffer;

    IO_LOG(TRACE, "pumping %zu bytes through pipe", nbytes);

    if (NULL == pipe) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    switch (pipe->status) {
        case IO_V2_STATUS_OK:
            break;
        case IO_V2_STATUS_ERROR:
			IO_LOG(ERROR, "pipe is in error state");
			return (-1);
        case IO_V2_STATUS_EOF:
			IO_LOG(ERROR, "pipe is in EOF state");
			return (-1);
        case IO_V2_STATUS_CLOSED:
			IO_LOG(ERROR, "pipe is in closed state");
			return (-1);
        default:
            IO_LOG(ERROR, "invalid pipe status");
            return (-1);
    }
	ctx = (t_io_v2_pipe_ctx *)pipe->ctx;
    buffer = ctx->buffer;
    upstream = ctx->upstream;
    downstream = ctx->downstream;

    if (nbytes > buffer->capacity) {
        IO_LOG(ERROR, "not enough buffer capacity");
        return (-1);
    }
    if (nbytes == 0) {
        IO_LOG(DEBUG, "nothing to pump");
        return (0);
    }
    if (upstream->status == IO_V2_STATUS_EOF) {
        IO_LOG(TRACE, "source is at EOF");
        return (-1);
    }
    if (downstream->status == IO_V2_STATUS_EOF) {
        IO_LOG(TRACE, "downstream is at EOF");
        return (-1);
    }

	/* Read from upstream into internal buffer */
    IO_LOG(TRACE, "reading from upstream");
    rbytes = ft_buffer_write_with_func(buffer, __io_v2_read_adapter, upstream, nbytes);
    if (rbytes < 0) {
		if (upstream->status == IO_V2_STATUS_EOF) {
			if (ft_buffer_is_empty(buffer)) {
				IO_LOG(TRACE, "upstream is at EOF and internal buffer is empty");
				pipe->status = IO_V2_STATUS_EOF;
				return (-1);
			} else {
				IO_LOG(TRACE, "upstream is at EOF but internal buffer is not empty");
				// Not error, just continue to write to downstream
			}
		}
		else {
			IO_LOG(ERROR, "read from upstream failed");
			pipe->status = IO_V2_STATUS_ERROR;
			return (-1);
		}
    }
	else {
    	IO_LOG(TRACE, "read %zu bytes from upstream", rbytes);
	}

	/* Write from internal buffer to downstream */
	if (ft_buffer_is_empty(buffer)) {
		IO_LOG(TRACE, "internal buffer is empty, nothing to write to downstream");
		return (0);
	}
	else {
		IO_LOG(TRACE, "writing to downstream");
		wbytes = ft_buffer_read_with_func(buffer, __io_v2_write_adapter, downstream, nbytes);
		if (wbytes < 0) {
			IO_LOG(ERROR, "write to downstream failed");
			pipe->status = IO_V2_STATUS_ERROR;
			return (-1);
		}
		else {
			IO_LOG(TRACE, "wrote %zu bytes to downstream", wbytes);
		}
	}
    return (wbytes);
}

ssize_t io_v2_pipe_flush(t_io_v2_pipe *pipe)
{
	t_io_v2_pipe_ctx *ctx;
    t_io_v2_stream *downstream;
	t_buffer *buffer;
	ssize_t wbytes;

	IO_LOG(TRACE, "flushing pipe");

    if (NULL == pipe) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    switch (pipe->status) {
        case IO_V2_STATUS_OK:
        case IO_V2_STATUS_EOF:
            break;
        case IO_V2_STATUS_ERROR:
            IO_LOG(ERROR, "pipe is in error state");
            return (-1);
        case IO_V2_STATUS_CLOSED:
            IO_LOG(ERROR, "pipe is in closed state");
            return (-1);
        default:
            IO_LOG(ERROR, "invalid pipe status");
            return (-1);
    }
	ctx = (t_io_v2_pipe_ctx *)pipe->ctx;
    downstream = ctx->downstream;
    buffer = ctx->buffer;

    if (downstream->status != IO_V2_STATUS_OK) {
        IO_LOG(ERROR, "downstream is not OK, status=%#x", downstream->status);
		pipe->status = IO_V2_STATUS_ERROR;
        return (-1);
    }
	if (ft_buffer_is_empty(buffer)) {
		IO_LOG(TRACE, "internal buffer is empty, nothing to flush");
		return (0);
	}
	else {
		IO_LOG(TRACE, "flushing %zu bytes to downstream", ft_buffer_used(buffer));
		wbytes = ft_buffer_read_with_func(buffer, __io_v2_write_adapter, downstream, ft_buffer_used(buffer));
		if (wbytes < 0) {
			IO_LOG(ERROR, "flush to downstream failed");
			pipe->status = IO_V2_STATUS_ERROR;
			return (-1);
		}
		IO_LOG(TRACE, "flushed %zu bytes to downstream", wbytes);
	}
    return (wbytes);
}

/*
 * io_v2_pipe_close: Closes and frees the pipe structure.
 *
 * NOTE: This function does NOT close the upstream or downstream streams.
 * The caller is responsible for closing those streams separately if needed.
 * The pipe does not own the streams - they may be shared or used elsewhere.
 */
ssize_t io_v2_pipe_close(t_io_v2_pipe *pipe)
{
	t_io_v2_pipe_ctx *ctx;

	IO_LOG(TRACE, "closing pipe");

    if (NULL == pipe) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    switch (pipe->status) {
        case IO_V2_STATUS_OK:
        case IO_V2_STATUS_EOF:
            break;
        case IO_V2_STATUS_ERROR:
            IO_LOG(ERROR, "pipe is in error state");
            return (-1);
        case IO_V2_STATUS_CLOSED:
            IO_LOG(ERROR, "pipe is in closed state");
            return (-1);
        default:
            IO_LOG(ERROR, "invalid pipe status");
            return (-1);
    }
	ctx = (t_io_v2_pipe_ctx *)pipe->ctx;
    ft_buffer_del(ctx->buffer);
	SSL_FREE(ctx);
	pipe->ctx = NULL;
	pipe->status = IO_V2_STATUS_CLOSED;

	IO_LOG(TRACE, "closed pipe");

    return (0);
}
