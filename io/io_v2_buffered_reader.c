#include <io.h>

typedef struct s_io_v2_buffered_reader_ctx {
	t_io_v2_stream *upstream;
	t_buffer *buffer;
} t_io_v2_buffered_reader_ctx;

static ssize_t  __io_v2_buffered_stream_read(void *vctx, const char *buf, size_t nbytes);
static ssize_t  __io_v2_buffered_stream_close(void *vctx);

int io_v2_buffered_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream, size_t capacity)
{
    const t_io_v2_interface interface = {
        .read = __io_v2_buffered_stream_read,
        .close = __io_v2_buffered_stream_close,
    };
    t_io_v2_buffered_reader_ctx *ctx;

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
    }
    if (NULL == upstream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
    }
    if (capacity == 0) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_v2_buffered_reader_ctx));
    ctx->upstream = upstream;
    ctx->buffer = ft_buffer_new(capacity);

    SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    (*stream)->ctx = ctx;
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_READ | IO_V2_FLAG_CLOSE;

    return (SSL_OK);
}

static ssize_t __io_v2_buffered_stream_read(void *vctx, const char *buf, size_t nbytes)
{
    t_io_v2_buffered_reader_ctx *ctx;
    ssize_t wbytes;

    ctx = (t_io_v2_buffered_reader_ctx *)vctx;

    // stub, not complete, pipe from upstream to buffer
    wbytes = ft_buffer_write_with_func(ctx->buffer, ctx->upstream->interface.read, ctx->upstream->ctx, nbytes);
    if (wbytes < 0) {
        return (IO_V2_STATUS_ERROR);
    }
    return (wbytes);
}

static ssize_t __io_v2_buffered_stream_close(void *vctx)
{
    t_io_v2_buffered_reader_ctx *ctx;

    ctx = (t_io_v2_buffered_reader_ctx *)vctx;

    IO_LOG(TRACE, "closing buffered stream");

    if (IO_V2_STATUS_OK != io_v2_close(ctx->upstream)) {
        IO_LOG(ERROR, "failed to close underlying stream");
        return (IO_V2_STATUS_ERROR);
    }
    ctx->upstream = NULL;

    if (NULL != ctx->buffer) {
        ft_buffer_del(ctx->buffer);
        ctx->buffer = NULL;
    }
    ctx->buffer = NULL;
    SSL_FREE(ctx);
    IO_LOG(TRACE, "buffered stream closed");

    return (IO_V2_STATUS_OK);
}