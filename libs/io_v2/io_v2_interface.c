#include <libft_v2.h>

# define __IO_INVALID_INTFC_ERROR	"interface is invalid"

# define __IO_CLOSED_STATUS_ERROR	"stream is closed"
# define __IO_CLOSED_FINISHED_ERROR	"stream is finished"
# define __IO_ERROR_STATUS_ERROR	"stream is in error state"
# define __IO_EOF_STATUS_ERROR		"stream is at EOF"
# define __IO_INVALID_STATUS_ERROR	"stream has invalid status"

ssize_t io_v2_read(t_io_v2_stream *stream, void *buf, size_t nbytes)
{
    ssize_t result;

    IO_LOG(TRACE, "reading from stream %p with buffer %p, nbytes=%zu", stream, buf, nbytes);

    if (NULL == stream) {
        IO_LOG(ERROR, IO_INPUT_ERROR);
        return (-1);
    }
    if (!FLAG(IO_V2_FLAG_READ, stream->flags)) {
        IO_LOG(ERROR, "stream is not readable");
        return (-1);
    }
    switch (stream->status) {
    case IO_V2_STATUS_OK:
        break;
    case IO_V2_STATUS_ERROR:
		IO_LOG(ERROR, __IO_ERROR_STATUS_ERROR);
        return (-1);
    case IO_V2_STATUS_CLOSED:
		IO_LOG(ERROR, __IO_CLOSED_STATUS_ERROR);
        return (-1);
    case IO_V2_STATUS_EOF:
		IO_LOG(ERROR, __IO_EOF_STATUS_ERROR);
        return (-1);
    case IO_V2_STATUS_FINISHED:
    default:
        IO_LOG(ERROR, __IO_INVALID_STATUS_ERROR);
        return (-1);
    }
    if (NULL == stream->interface.read) {
        stream->status = IO_V2_STATUS_ERROR;
        IO_LOG(ERROR, __IO_INVALID_INTFC_ERROR);
        return (-1);
    }
    result = stream->interface.read(stream->ctx, buf, nbytes);
    if (result < 0) {
        if (result == IO_V2_STATUS_EOF) {
			IO_LOG(TRACE, "read reached EOF");
            stream->status = IO_V2_STATUS_EOF;
        } else {
			IO_LOG(ERROR, "interface read error");
			stream->status = IO_V2_STATUS_ERROR;
        }
        return (-1);
    }
	IO_LOG(TRACE, "read %zu bytes", result);

    return (result);
}

ssize_t io_v2_read_all(t_io_v2_stream *stream, void *buf, size_t nbytes)
{
	// BLOCKING
	// Attempt to read all bytes in one call.
	ssize_t trbytes = 0;
	ssize_t rbytes = 0;
	while (trbytes < nbytes) {
		rbytes = io_v2_read(stream, buf + trbytes, nbytes - trbytes);
		if (rbytes < 0) {
			break;
		}
		trbytes += rbytes;
	}
	switch (stream->status) {
	case IO_V2_STATUS_OK:
		return (trbytes);
	case IO_V2_STATUS_EOF:
		IO_LOG(TRACE, "eof reached");
		return (0);
	case IO_V2_STATUS_FINISHED:
	case IO_V2_STATUS_ERROR:
	case IO_V2_STATUS_CLOSED:
	default:
		IO_LOG(ERROR, IO_READ_ERROR);
		return (-1);
	}
	return (trbytes);
}

ssize_t io_v2_write(t_io_v2_stream *stream, const void *buf, size_t nbytes)
{
    ssize_t result;

    IO_LOG(TRACE, "writing to stream %p with buffer %p, nbytes=%zu", stream, buf, nbytes);

    if (NULL == stream) {
        IO_LOG(ERROR, IO_INPUT_ERROR);
        return (-1);
    }
    if (!FLAG(IO_V2_FLAG_WRITE, stream->flags)) {
        IO_LOG(ERROR, "stream is not writable");
        return (-1);
    }
    switch (stream->status) {
    case IO_V2_STATUS_OK:
        break;
    case IO_V2_STATUS_ERROR:
		IO_LOG(ERROR, __IO_ERROR_STATUS_ERROR);
        return (-1);
    case IO_V2_STATUS_CLOSED:
		IO_LOG(ERROR, __IO_CLOSED_STATUS_ERROR);
        return (-1);
    case IO_V2_STATUS_FINISHED:
		IO_LOG(ERROR, __IO_CLOSED_FINISHED_ERROR);
        return (-1);
    case IO_V2_STATUS_EOF:
    default:
        IO_LOG(ERROR, __IO_INVALID_STATUS_ERROR);
        return (-1);
    }
    if (NULL == stream->interface.write) {
        stream->status = IO_V2_STATUS_ERROR;
        IO_LOG(ERROR, __IO_INVALID_INTFC_ERROR);
        return (-1);
    }
    result = stream->interface.write(stream->ctx, buf, nbytes);
    if (result < 0) {
        IO_LOG(ERROR, "interface write error");
        stream->status = IO_V2_STATUS_ERROR;
        return (-1);
    }
	IO_LOG(TRACE, "wrote %zd bytes", result);

    return (result);
}

ssize_t io_v2_write_all(t_io_v2_stream *stream, const void *buf, size_t nbytes)
{
	// BLOCKING
	// Attempt to write all bytes in one call.
	ssize_t twbytes = 0;
	ssize_t wbytes = 0;
	while (twbytes < nbytes) {
		wbytes = io_v2_write(stream, buf + twbytes, nbytes - twbytes);
		if (wbytes < 0) {
			break;
		}
		twbytes += wbytes;
	}
	switch (stream->status) {
	case IO_V2_STATUS_OK:
		return (twbytes);
	case IO_V2_STATUS_EOF:
	case IO_V2_STATUS_ERROR:
	case IO_V2_STATUS_CLOSED:
	case IO_V2_STATUS_FINISHED:
	default:
		IO_LOG(ERROR, IO_WRITE_ERROR);
		return (-1);
	}
	return (twbytes);
}

ssize_t io_v2_finish(t_io_v2_stream *stream)
{
    ssize_t result;

    IO_LOG(TRACE, "finishing stream %p", stream);

    if (NULL == stream) {
        IO_LOG(ERROR, IO_INPUT_ERROR);
        return (-1);
    }
    if (!FLAG(IO_V2_FLAG_FINISH, stream->flags)) {
        IO_LOG(TRACE, "stream is not finishable");
        return (0);
    }
    switch (stream->status) {
    case IO_V2_STATUS_OK:
        break;
    case IO_V2_STATUS_ERROR:
		IO_LOG(ERROR, __IO_ERROR_STATUS_ERROR);
        return (-1);
    case IO_V2_STATUS_CLOSED:
		IO_LOG(ERROR, __IO_CLOSED_STATUS_ERROR);
        return (-1);
    case IO_V2_STATUS_FINISHED:
		IO_LOG(ERROR, __IO_CLOSED_FINISHED_ERROR);
        return (-1);
    case IO_V2_STATUS_EOF:
    default:
        IO_LOG(ERROR, __IO_INVALID_STATUS_ERROR);
        return (-1);
    }
    if (NULL == stream->interface.finish) {
        stream->status = IO_V2_STATUS_ERROR;
        IO_LOG(ERROR, __IO_INVALID_INTFC_ERROR);
        return (-1);
    }
    result = stream->interface.finish(stream->ctx);
    if (result < 0) {
        IO_LOG(ERROR, "interface finish error");
        stream->status = IO_V2_STATUS_ERROR;
        return (-1);
    }
	IO_LOG(TRACE, "wrote %zd bytes", result);
	stream->status = IO_V2_STATUS_FINISHED;
	IO_LOG(TRACE, "finished stream");

    return (result);
}

ssize_t io_v2_flush(t_io_v2_stream *stream)
{
    ssize_t result;

    IO_LOG(TRACE, "flushing stream %p", stream);

    if (NULL == stream) {
        IO_LOG(ERROR, IO_INPUT_ERROR);
        return (-1);
    }
    if (!FLAG(IO_V2_FLAG_FLUSH, stream->flags)) {
        IO_LOG(TRACE, "stream is not flushable");
        return (0);
    }
    switch (stream->status) {
    case IO_V2_STATUS_OK:
        break;
    case IO_V2_STATUS_ERROR:
		IO_LOG(ERROR, __IO_ERROR_STATUS_ERROR);
        return (-1);
    case IO_V2_STATUS_CLOSED:
		IO_LOG(ERROR, __IO_CLOSED_STATUS_ERROR);
        return (-1);
    case IO_V2_STATUS_FINISHED:
        IO_LOG(ERROR, __IO_CLOSED_FINISHED_ERROR);
        return (-1);
    case IO_V2_STATUS_EOF:
    default:
        IO_LOG(ERROR, __IO_INVALID_STATUS_ERROR);
        return (-1);
    }
    if (NULL == stream->interface.flush) {
        stream->status = IO_V2_STATUS_ERROR;
        IO_LOG(ERROR, __IO_INVALID_INTFC_ERROR);
        return (-1);
    }
	result = stream->interface.flush(stream->ctx);
	if (result < 0) {
		IO_LOG(ERROR, "interface flush error");
		stream->status = IO_V2_STATUS_ERROR;
		return (-1);
	}
	IO_LOG(TRACE, "flushed %zd bytes", result);

    return (result);
}

ssize_t io_v2_close(t_io_v2_stream *stream)
{
    ssize_t result;

    IO_LOG(TRACE, "closing stream %p", stream);

    if (NULL == stream) {
        IO_LOG(ERROR, IO_INPUT_ERROR);
        return (-1);
    }
    if (!FLAG(IO_V2_FLAG_CLOSE, stream->flags)) {
        IO_LOG(TRACE, "stream is not closable");
        return (0);
    }
	/* All known states are ok except closed state */
    switch (stream->status) {
   	case IO_V2_STATUS_OK:
	case IO_V2_STATUS_EOF:
    case IO_V2_STATUS_ERROR:
    case IO_V2_STATUS_FINISHED:
        break;
    case IO_V2_STATUS_CLOSED:
		IO_LOG(ERROR, __IO_CLOSED_STATUS_ERROR);
        return (-1);
    default:
        IO_LOG(ERROR, __IO_INVALID_STATUS_ERROR);
        return (-1);
    }
    if (NULL == stream->interface.close) {
        stream->status = IO_V2_STATUS_ERROR;
        IO_LOG(ERROR, __IO_INVALID_INTFC_ERROR);
        return (-1);
    }
    result = stream->interface.close(stream->ctx);
    if (result < 0) {
		IO_LOG(ERROR, "interface close error");
        stream->status = IO_V2_STATUS_ERROR;
        return (-1);
    }
	stream->status = IO_V2_STATUS_CLOSED;
	IO_LOG(TRACE, "closed stream");

    return (result);
}
