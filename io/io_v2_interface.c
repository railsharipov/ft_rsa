#include <io.h>

ssize_t io_v2_read(t_io_v2_stream *stream, void *buf, size_t nbytes)
{
    ssize_t result;

    IO_LOG(TRACE, "reading from stream %p with buffer %p, nbytes=%zu", stream, buf, nbytes);

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    if (!SSL_FLAG(IO_V2_FLAG_READ, stream->flags)) {
        IO_LOG(ERROR, "stream is not readable");
        return (-1);
    }
    switch (stream->status) {
        case IO_V2_STATUS_OK:
            break;
        case IO_V2_STATUS_ERROR:
			IO_LOG(ERROR, "stream is in error state");
            return (-1);
        case IO_V2_STATUS_CLOSED:
			IO_LOG(ERROR, "stream is closed");
            return (-1);
        case IO_V2_STATUS_EOF:
			IO_LOG(ERROR, "stream is at EOF");
            return (-1);
        default:
            IO_LOG(ERROR, "invalid stream status %#x", stream->status);
            return (-1);
    }
    if (NULL == stream->interface.read) {
        stream->status = IO_V2_STATUS_ERROR;
        IO_LOG(ERROR, "read function is not implemented");
        return (-1);
    }
    result = stream->interface.read(stream->ctx, buf, nbytes);
    if (result < 0) {
        if (result == IO_V2_STATUS_EOF) {
			IO_LOG(TRACE, "read reached EOF");
            stream->status = IO_V2_STATUS_EOF;
        } else {
			IO_LOG(ERROR, "read failed");
			stream->status = IO_V2_STATUS_ERROR;
        }
        return (-1);
    }
	IO_LOG(TRACE, "read %zu bytes", result);

    return (result);
}

ssize_t io_v2_write(t_io_v2_stream *stream, const void *buf, size_t nbytes)
{
    ssize_t result;

    IO_LOG(TRACE, "writing to stream %p with buffer %p, nbytes=%zu", stream, buf, nbytes);

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    if (!SSL_FLAG(IO_V2_FLAG_WRITE, stream->flags)) {
        IO_LOG(ERROR, "stream is not writable");
        return (-1);
    }
    switch (stream->status) {
        case IO_V2_STATUS_OK:
            break;
        case IO_V2_STATUS_ERROR:
			IO_LOG(ERROR, "stream is in error state");
            return (-1);
        case IO_V2_STATUS_CLOSED:
			IO_LOG(ERROR, "stream is closed");
            return (-1);
        default:
            IO_LOG(ERROR, "invalid stream status %#x", stream->status);
            return (-1);
    }
    if (NULL == stream->interface.write) {
        stream->status = IO_V2_STATUS_ERROR;
        IO_LOG(ERROR, "write function is not implemented");
        return (-1);
    }
    result = stream->interface.write(stream->ctx, buf, nbytes);
    if (result < 0) {
        stream->status = IO_V2_STATUS_ERROR;
        IO_LOG(ERROR, "write failed");
        return (-1);
    }
	IO_LOG(TRACE, "wrote %zu bytes", result);

    return (result);
}

ssize_t io_v2_flush(t_io_v2_stream *stream)
{
    ssize_t result;

    IO_LOG(TRACE, "flushing stream %p", stream);

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    if (!SSL_FLAG(IO_V2_FLAG_FLUSH, stream->flags)) {
        IO_LOG(ERROR, "stream is not flushable");
        return (-1);
    }
    if (!SSL_FLAG(IO_V2_FLAG_WRITE, stream->flags)) {
        IO_LOG(ERROR, "stream is not writable");
        return (-1);
    }
    switch (stream->status) {
        case IO_V2_STATUS_OK:
            break;
        case IO_V2_STATUS_ERROR:
			IO_LOG(ERROR, "stream is in error state");
            return (-1);
        case IO_V2_STATUS_CLOSED:
			IO_LOG(ERROR, "stream is closed");
            return (-1);
        default:
            IO_LOG(ERROR, "invalid stream status %#x", stream->status);
            return (-1);
    }
    if (NULL == stream->interface.flush) {
        stream->status = IO_V2_STATUS_ERROR;
        IO_LOG(ERROR, "flush function is not implemented");
        return (-1);
    }
	result = stream->interface.flush(stream->ctx);
	if (result < 0) {
		stream->status = IO_V2_STATUS_ERROR;
		IO_LOG(ERROR, "flush failed");
		return (-1);
	}
	IO_LOG(TRACE, "flushed %zu bytes", result);

    return (result);
}

ssize_t io_v2_close(t_io_v2_stream *stream)
{
    ssize_t result;

    IO_LOG(TRACE, "closing stream %p", stream);

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    if (!SSL_FLAG(IO_V2_FLAG_CLOSE, stream->flags)) {
        IO_LOG(ERROR, "stream is not closable");
        return (-1);
    }
    switch (stream->status) {
        case IO_V2_STATUS_OK:
		case IO_V2_STATUS_EOF:
            break;
        case IO_V2_STATUS_ERROR:
			IO_LOG(ERROR, "stream is in error state");
            return (-1);
        case IO_V2_STATUS_CLOSED:
			IO_LOG(ERROR, "stream is closed");
            return (-1);
        default:
            IO_LOG(ERROR, "invalid stream status %#x", stream->status);
            return (-1);
    }
	if (SSL_FLAG(IO_V2_FLAG_FLUSH, stream->flags)) {
		result = io_v2_flush(stream);
		if (result < 0) {
			IO_LOG(ERROR, "failed to flush stream on close");
			return (-1);
		}
		IO_LOG(TRACE, "flushed stream on close");
	}
    if (NULL == stream->interface.close) {
        stream->status = IO_V2_STATUS_ERROR;
        IO_LOG(ERROR, "close function is not implemented");
        return (-1);
    }
    result = stream->interface.close(stream->ctx);
    if (result < 0) {
		IO_LOG(ERROR, "close failed");
        stream->status = IO_V2_STATUS_ERROR;
        return (-1);
    }
	stream->status = IO_V2_STATUS_CLOSED;
	IO_LOG(TRACE, "closed stream");

    return (result);
}