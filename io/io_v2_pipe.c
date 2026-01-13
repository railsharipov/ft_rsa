// #include <io.h>
// #include <libft/std.h>
// #include <libft/alloc.h>

// int	io_v2_stream(t_io_v2_stream **stream, void *ctx, const t_io_v2_interface interface, enum e_io_v2_mode mode)
// {
// 	IO_LOG(TRACE, "io stream with ctx=%p, interface=%p, mode=%d", ctx, interface, mode);

// 	if (NULL == stream) {
// 		IO_LOG(ERROR, INVALID_INPUT_ERROR);
// 		return (SSL_ERR);
// 	}
// 	if (NULL == ctx) {
// 		IO_LOG(ERROR, "context is not specified");
// 		return (SSL_ERR);
// 	}
// 	switch (mode) {
// 		case IO_V2_MODE_READ:
// 		case IO_V2_MODE_WRITE:
// 			break;
// 		default:
// 			IO_LOG(ERROR, "invalid i/o mode %#x", mode);
// 			return (SSL_ERR);
// 	}
// 	SSL_ALLOC(*stream, sizeof(t_io_v2_stream));
// 	(*stream)->ctx = ctx;
// 	(*stream)->interface = interface;
// 	(*stream)->mode = mode;
// 	return (SSL_OK);
// }

// ssize_t	io_v2_read(t_io_v2_stream *stream, char *buf, size_t nbytes)
// {
// 	if (NULL == stream) {
// 		IO_LOG(ERROR, INVALID_INPUT_ERROR);
// 		return (-1);
// 	}
// 	if (stream->mode != IO_V2_MODE_READ) {
// 		IO_LOG(ERROR, "invalid i/o mode %#x, expected %#x", stream->mode, IO_V2_MODE_READ);
// 		return (-1);
// 	}
// 	if (NULL == stream->interface.read) {
// 		IO_LOG(ERROR, "read function is not specified");
// 		return (-1);
// 	}
// 	IO_LOG(TRACE, "reading %zu bytes", nbytes);
// 	return (stream->interface.read(stream->ctx, buf, nbytes));
// }

// ssize_t	io_v2_write(t_io_v2_stream *stream, const char *buf, size_t nbytes)
// {
// 	if (NULL == stream) {
// 		IO_LOG(ERROR, INVALID_INPUT_ERROR);
// 		return (-1);
// 	}
// 	if (stream->mode != IO_V2_MODE_WRITE) {
// 		IO_LOG(ERROR, "invalid i/o mode %#x, expected %#x", stream->mode, IO_V2_MODE_WRITE);
// 		return (-1);
// 	}
// 	if (NULL == stream->interface.write) {
// 		IO_LOG(ERROR, "write function is not specified");
// 		return (-1);
// 	}
// 	IO_LOG(TRACE, "writing %zu bytes", nbytes);
// 	return (stream->interface.write(stream->ctx, buf, nbytes));
// }

// void	io_v2_close(t_io_v2_stream *stream)
// {
// 	if (NULL == stream) {
// 		IO_LOG(ERROR, INVALID_INPUT_ERROR);
// 		return;
// 	}
// 	if (NULL == stream->interface.close) {
// 		IO_LOG(ERROR, "close function is not specified");
// 		return;
// 	}
// 	IO_LOG(TRACE, "closing stream");
// 	return (stream->interface.close(stream->ctx));
// }
