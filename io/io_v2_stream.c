#include <io.h>
#include <libft/std.h>
#include <libft/alloc.h>

int	io_v2_stream(t_io_v2_stream **stream, void *ctx, const t_io_v2_interface interface, t_io_v2_flag flags)
{
	IO_LOG(TRACE, "io stream with ctx=%p, interface=%p, flags=%#x", ctx, interface, flags);

	if (NULL == stream) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == ctx) {
		IO_LOG(ERROR, "context is not specified");
		return (SSL_ERR);
	}
	if (!SSL_FLAG(IO_V2_FLAG_READ, flags) && !SSL_FLAG(IO_V2_FLAG_WRITE, flags)) {
		IO_LOG(ERROR, "neither read nor write flag is set");
		return (SSL_ERR);
	}
	if (SSL_FLAG(IO_V2_FLAG_READ, flags) && SSL_FLAG(IO_V2_FLAG_WRITE, flags)) {
		IO_LOG(ERROR, "both read and write flags are set");
		return (SSL_ERR);
	}
	SSL_ALLOC(*stream, sizeof(t_io_v2_stream));
	(*stream)->ctx = ctx;
	(*stream)->interface = interface;
	(*stream)->type = IO_V2_TYPE_BASIC;
	(*stream)->flags = flags;
	(*stream)->status = IO_V2_STATUS_OK;
	return (SSL_OK);
}

ssize_t	io_v2_read(t_io_v2_stream *stream, char *buf, size_t nbytes)
{
	if (NULL == stream) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	switch (stream->status) {
		case IO_V2_STATUS_OK:
			break;
		case IO_V2_STATUS_CLOSED:
			IO_LOG(ERROR, "stream is closed");
			return (-1);
		case IO_V2_STATUS_EOF:
			IO_LOG(TRACE, "stream is at EOF");
			return (0);
		default:
			IO_LOG(ERROR, "stream is not in a valid state");
			return (-1);
	}
	if (!SSL_FLAG(IO_V2_FLAG_READ, stream->flags)) {
		IO_LOG(ERROR, "stream is not readable");
		return (-1);
	}
	if (NULL == stream->interface.read) {
		IO_LOG(ERROR, "read function is not specified");
		return (-1);
	}
	IO_LOG(TRACE, "reading %zu bytes", nbytes);
	return (stream->interface.read(stream->ctx, buf, nbytes));
}

ssize_t	io_v2_write(t_io_v2_stream *stream, const char *buf, size_t nbytes)
{
	if (NULL == stream) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	switch (stream->status) {
		case IO_V2_STATUS_OK:
			break;
		case IO_V2_STATUS_CLOSED:
			IO_LOG(ERROR, "stream is closed");
			return (-1);
		case IO_V2_STATUS_EOF:
			IO_LOG(TRACE, "stream is at EOF");
			return (0);
		default:
			IO_LOG(ERROR, "stream is not in a valid state");
			return (-1);
	}
	if (!SSL_FLAG(IO_V2_FLAG_WRITE, stream->flags)) {
		IO_LOG(ERROR, "stream is not writable");
		return (-1);
	}
	if (NULL == stream->interface.write) {
		IO_LOG(ERROR, "write function is not specified");
		return (-1);
	}
	IO_LOG(TRACE, "writing %zu bytes", nbytes);
	return (stream->interface.write(stream->ctx, buf, nbytes));
}

ssize_t	io_v2_flush(t_io_v2_stream *stream, const char *buf, size_t nbytes)
{
	if (NULL == stream) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	switch (stream->status) {
		case IO_V2_STATUS_OK:
			break;
		case IO_V2_STATUS_CLOSED:
			IO_LOG(ERROR, "stream is closed");
			return (-1);
		case IO_V2_STATUS_EOF:
			IO_LOG(TRACE, "stream is at EOF");
			return (0);
		default:
			IO_LOG(ERROR, "stream is not in a valid state");
			return (-1);
	}
	if (!SSL_FLAG(IO_V2_FLAG_FLUSH, stream->flags)) {
		IO_LOG(ERROR, "stream is not flushable");
		return (-1);
	}
	if (NULL == stream->interface.flush) {
		IO_LOG(ERROR, "flush function is not specified");
		return (-1);
	}
	IO_LOG(TRACE, "flushing stream");
	return (stream->interface.flush(stream->ctx, buf, nbytes));
}

void	io_v2_close(t_io_v2_stream *stream)
{
	if (NULL == stream) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return;
	}
	switch (stream->status) {
		case IO_V2_STATUS_OK:
		case IO_V2_STATUS_EOF:
			break;
		case IO_V2_STATUS_CLOSED:
			IO_LOG(TRACE, "stream is closed");
			return;
		default:
			IO_LOG(ERROR, "stream is not in a valid state");
			return;
	}
	if (!SSL_FLAG(IO_V2_FLAG_CLOSE, stream->flags)) {
		IO_LOG(ERROR, "stream is not closeable");
		return;
	}
	if (NULL == stream->interface.close) {
		IO_LOG(ERROR, "no close function specified for stream");
		return;
	}
	IO_LOG(TRACE, "closing stream");
	stream->interface.close(stream->ctx);
	stream->status = IO_V2_STATUS_CLOSED;
}
