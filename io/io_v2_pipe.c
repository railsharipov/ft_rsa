#include <io.h>

static ssize_t __io_v2_read_adapter(void *ctx, void *buf, size_t nbytes);
static ssize_t __io_v2_write_adapter(void *ctx, const void *buf, size_t nbytes);

int io_v2_pipe_unidir(t_io_v2_pipe **pipe, t_io_v2_stream *upstream, t_io_v2_stream *downstream, size_t capacity)
{
    if (NULL == upstream || NULL == downstream) {
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
    (*pipe)->upstream = upstream;
    (*pipe)->downstream = downstream;
    (*pipe)->type = IO_V2_PIPE_TYPE_UNIDIR;
    (*pipe)->status = IO_V2_STATUS_OK;
    (*pipe)->buffer = ft_buffer_new(capacity);
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
	t_io_v2_stream *stream;

	stream = (t_io_v2_stream *)ctx;
	return (io_v2_read(stream, buf, nbytes));
}

static ssize_t __io_v2_write_adapter(void *ctx, const void *buf, size_t nbytes)
{
	t_io_v2_stream *stream;

	stream = (t_io_v2_stream *)ctx;
	return (io_v2_write(stream, buf, nbytes));
}

ssize_t io_v2_pipe_pump(t_io_v2_pipe *pipe, size_t nbytes)
{
    t_io_v2_stream *upstream, *downstream;
    ssize_t rbytes, wbytes;
    t_buffer *buffer;

    buffer = pipe->buffer;
    upstream = pipe->upstream;
    downstream = pipe->downstream;

    IO_LOG(TRACE, "pumping pipe with upstream=%p, downstream=%p, buffer=%p, nbytes=%zu", upstream, downstream, buffer, nbytes);

    if (NULL == pipe) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    if (nbytes > buffer->capacity) {
        IO_LOG(ERROR, "not enough buffer capacity");
        return (-1);
    }
    if (nbytes == 0) {
        IO_LOG(DEBUG, "nothing to pump");
        return (0);
    }
    switch (pipe->status) {
        case IO_V2_STATUS_OK:
            break;
        case IO_V2_STATUS_ERROR:
        case IO_V2_STATUS_EOF:
            IO_LOG(ERROR, "pipe is in error/EOF state");
            return (-1);
        default:
            IO_LOG(ERROR, "invalid pipe status");
            return (-1);
    }

    if (upstream->status == IO_V2_STATUS_EOF) {
        IO_LOG(TRACE, "source is at EOF");
        return (-1);
    }
    if (downstream->status == IO_V2_STATUS_EOF) {
        IO_LOG(TRACE, "downstream is at EOF");
        return (-1);
    }

    IO_LOG(TRACE, "reading from upstream");
    rbytes = ft_buffer_write_with_func(buffer, __io_v2_read_adapter, upstream, nbytes);
    if (rbytes < 0) {
		if (upstream->status == IO_V2_STATUS_EOF) {
			pipe->status = IO_V2_STATUS_EOF;
			return (-1);
		}
        IO_LOG(ERROR, "read from upstream failed");
        pipe->status = IO_V2_STATUS_ERROR;
        return (-1);
    }
    IO_LOG(TRACE, "read %zu bytes from upstream", rbytes);

    IO_LOG(TRACE, "writing to downstream");
    wbytes = ft_buffer_read_with_func(buffer, __io_v2_write_adapter, downstream, rbytes);
    if (wbytes < 0) {
        IO_LOG(ERROR, "write to downstream failed");
        pipe->status = IO_V2_STATUS_ERROR;
        return (-1);
    }
    IO_LOG(TRACE, "wrote %zu bytes to downstream", wbytes);

    return (wbytes);
}

ssize_t io_v2_pipe_flush(t_io_v2_pipe *pipe)
{
    t_io_v2_stream *downstream;
	ssize_t wbytes;

    downstream = pipe->downstream;

    if (NULL == pipe) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    if (ft_buffer_is_empty(pipe->buffer)) {
        IO_LOG(DEBUG, "buffer is empty, nothing to flush");
        return (0);
    }
    switch (pipe->status) {
        case IO_V2_STATUS_OK:
        case IO_V2_STATUS_EOF:
            break;
        case IO_V2_STATUS_ERROR:
            IO_LOG(ERROR, "pipe is in error state");
            return (-1);
        default:
            IO_LOG(ERROR, "invalid pipe status");
            return (-1);
    }
    if (downstream->status != IO_V2_STATUS_OK) {
        IO_LOG(ERROR, "downstream is not OK, status=%#x", downstream->status);
        return (-1);
    }
    IO_LOG(TRACE, "flushing %zu bytes to downstream", ft_buffer_used(pipe->buffer));
    wbytes = ft_buffer_read_with_func(pipe->buffer, __io_v2_write_adapter, downstream, ft_buffer_used(pipe->buffer));
    if (wbytes < 0) {
        IO_LOG(ERROR, "flush to downstream failed");
        pipe->status = IO_V2_STATUS_ERROR;
        return (-1);
    }
    IO_LOG(TRACE, "flushed %zu bytes to downstream", wbytes);

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
    if (NULL == pipe) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    ft_buffer_del(pipe->buffer);
    SSL_FREE(pipe);
    return (0);
}
