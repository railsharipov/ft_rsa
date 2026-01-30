#include <io.h>

int io_v2_stream(t_io_v2_stream **stream, t_io_v2_interface interface, t_io_v2_flag flags, void *ctx)
{
	if (NULL == stream) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (SSL_FLAG(IO_V2_FLAG_READ, flags) && NULL == interface.read) {
		SSL_LOG(ERROR, "no read interface");
		return (SSL_ERR);
	}
	if (SSL_FLAG(IO_V2_FLAG_WRITE, flags) && NULL == interface.write) {
		SSL_LOG(ERROR, "no write interface");
		return (SSL_ERR);
	}
	if (SSL_FLAG(IO_V2_FLAG_FLUSH, flags) && NULL == interface.flush) {
		SSL_LOG(ERROR, "no flush interface");
		return (SSL_ERR);
	}
	if (SSL_FLAG(IO_V2_FLAG_CLOSE, flags) && NULL == interface.close) {
		SSL_LOG(ERROR, "no close interface");
		return (SSL_ERR);
	}
	SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
	(*stream)->interface = interface;
	(*stream)->flags = flags;
	(*stream)->status = IO_V2_STATUS_OK;
	(*stream)->ctx = ctx;

	return (SSL_OK);
}