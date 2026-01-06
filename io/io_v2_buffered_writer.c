#include <io.h>

typedef struct s_io_v2_buffered_writer_ctx {
	t_io_v2_stream *downstream;
	t_buffer *buffer;
} t_io_v2_buffered_writer_ctx;

static ssize_t  __io_v2_buffered_stream_write(void *vctx, const char *buf, size_t nbytes);
static ssize_t  __io_v2_buffered_stream_flush(void *vctx);
static ssize_t  __io_v2_buffered_stream_close(void *vctx);

int io_v2_buffered_writer(t_io_v2_stream **stream, t_io_v2_stream *downstream, size_t capacity)
{
    const t_io_v2_interface interface = {
        .write = __io_v2_buffered_stream_write,
        .flush = __io_v2_buffered_stream_flush,
        .close = __io_v2_buffered_stream_close,
    };
    t_io_v2_buffered_writer_ctx *ctx;

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
    }
    if (NULL == downstream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
    }
    if (capacity == 0) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_v2_buffered_writer_ctx));
    ctx->downstream = downstream;
    ctx->buffer = ft_buffer_new(capacity);

    SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    (*stream)->ctx = ctx;
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_WRITE | IO_V2_FLAG_FLUSH | IO_V2_FLAG_CLOSE;

    return (SSL_OK);
}

static ssize_t __io_v2_buffered_stream_write(void *vctx, const char *buf, size_t nbytes)
{
    t_io_v2_buffered_writer_ctx *ctx;
    ssize_t rbytes;

    ctx = (t_io_v2_buffered_writer_ctx *)vctx;

    // stub, not complete, pipe from buffer to downstream
    rbytes = ft_buffer_read_with_func(ctx->buffer, ctx->downstream->interface.write, ctx->downstream->ctx, nbytes);
    if (rbytes < 0) {
        return (IO_V2_STATUS_ERROR);
    }
    return (rbytes);
}

static ssize_t __io_v2_buffered_stream_flush(void *vctx)
{
    t_io_v2_buffered_writer_ctx *ctx;
    t_io_v2_stream *downstream;
    t_buffer *buffer;
    ssize_t wbytes;

    ctx = (t_io_v2_buffered_writer_ctx *)vctx;

    downstream = ctx->downstream;
    buffer = ctx->buffer;

    IO_LOG(TRACE, "flushing buffered stream with downstream=%p, buffer=%p, capacity=%zu", downstream, buffer, buffer->capacity);

    wbytes = ft_buffer_read_with_func(buffer, downstream->interface.write, downstream->ctx, ft_buffer_used(buffer));
    if (wbytes < 0) {
        IO_LOG(ERROR, "failed to flush buffered stream");
        return (IO_V2_STATUS_ERROR);
    }
    return (wbytes);
}

static ssize_t __io_v2_buffered_stream_close(void *vctx)
{
    t_io_v2_buffered_writer_ctx *ctx;

    ctx = (t_io_v2_buffered_writer_ctx *)vctx;

    IO_LOG(TRACE, "closing buffered stream");

    if (IO_V2_STATUS_OK != io_v2_close(ctx->downstream)) {
        IO_LOG(ERROR, "failed to close underlying stream");
        return (IO_V2_STATUS_ERROR);
    }
    ctx->downstream = NULL;

    if (NULL != ctx->buffer) {
        ft_buffer_del(ctx->buffer);
        ctx->buffer = NULL;
    }
    ctx->buffer = NULL;
    SSL_FREE(ctx);
    IO_LOG(TRACE, "buffered stream closed");

    return (IO_V2_STATUS_OK);
}