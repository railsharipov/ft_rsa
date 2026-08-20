#include <io.h>
#include <logger.h>

int io_v2_stream(t_io_v2_stream **stream, t_io_v2_interface interface, void *ctx)
{
	t_io_v2_flag flags = 0;

	if (NULL == stream) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL != interface.read) flags |= IO_V2_FLAG_READ;
	if (NULL != interface.write) flags |= IO_V2_FLAG_WRITE;
	if (NULL != interface.flush) flags |= IO_V2_FLAG_FLUSH;
	if (NULL != interface.finish) flags |= IO_V2_FLAG_FINISH;
	if (NULL != interface.close) flags |= IO_V2_FLAG_CLOSE;

	SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
	(*stream)->interface = interface;
	(*stream)->flags = flags;
	(*stream)->status = IO_V2_STATUS_OK;
	(*stream)->ctx = ctx;

	return (SSL_OK);
}
