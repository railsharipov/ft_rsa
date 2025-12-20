#include <io.h>
#include <libft/std.h>
#include <libft/alloc.h>

int	io_stream(t_io_stream **stream, void *ctx, const t_io_interface interface, enum e_io_mode mode)
{
	IO_LOG(TRACE, "io stream with ctx=%p, interface=%p, mode=%d", ctx, interface, mode);

	if (NULL == stream) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == ctx) {
		IO_LOG(ERROR, "context is not specified");
		return (SSL_ERR);
	}
	switch (mode) {
		case IO_MODE_READ:
		case IO_MODE_WRITE:
		case IO_MODE_PIPE:
			break;
		default:
			IO_LOG(ERROR, "invalid i/o mode %#x", mode);
			return (SSL_ERR);
	}
	SSL_ALLOC(*stream, sizeof(t_io_stream));
	(*stream)->ctx = ctx;
	(*stream)->interface = interface;
	(*stream)->mode = mode;
	return (SSL_OK);
}