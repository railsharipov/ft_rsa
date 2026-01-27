#include <io.h>

typedef struct s_io_v2_bytes_ctx {
    t_ostring	*ostring;
    size_t      seek;
} t_io_v2_bytes_ctx;

static ssize_t __io_v2_bytes_read(void *vctx, void *buf, size_t nbytes);
static ssize_t __io_v2_bytes_write(void *vctx, const void *buf, size_t nbytes);
static ssize_t __io_v2_bytes_close(void *vctx);

int io_v2_bytes_reader(t_io_v2_stream **stream, t_ostring *ostring)
{
    const t_io_v2_interface interface = {
        .read = __io_v2_bytes_read,
        .close = __io_v2_bytes_close,
    };
    t_io_v2_bytes_ctx *ctx;

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (NULL == ostring) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_v2_bytes_ctx));
    ctx->ostring = ostring;
    ctx->seek = 0;

    SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_READ | IO_V2_FLAG_CLOSE;
    (*stream)->status = IO_V2_STATUS_OK;
    (*stream)->ctx = ctx;

    return (SSL_OK);
}

int io_v2_bytes_writer(t_io_v2_stream **stream, t_ostring *ostring)
{
    const t_io_v2_interface interface = {
        .write = __io_v2_bytes_write,
        .close = __io_v2_bytes_close,
    };
    t_io_v2_bytes_ctx *ctx;

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (NULL == ostring) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_v2_bytes_ctx));
    ctx->ostring = ostring;
    ctx->seek = 0;

    SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_WRITE | IO_V2_FLAG_CLOSE;
    (*stream)->status = IO_V2_STATUS_OK;
    (*stream)->ctx = ctx;

    return (SSL_OK);
}

static ssize_t __io_v2_bytes_read(void *vctx, void *buf, size_t nbytes)
{
    t_io_v2_bytes_ctx *ctx;
    ssize_t rbytes;

    if (NULL == vctx || NULL == buf) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }
    if (nbytes == 0) {
		IO_LOG(TRACE, "buffer size is 0");
        return (0);
    }
	ctx = (t_io_v2_bytes_ctx *)vctx;

    if (ctx->seek >= ctx->ostring->size) {
        IO_LOG(TRACE, "EOF reached");
        return (IO_V2_STATUS_EOF);
    }
    rbytes = MIN(nbytes, ctx->ostring->size - ctx->seek);
    ft_memcpy(buf, ctx->ostring->content + ctx->seek, rbytes);
    ctx->seek += rbytes;

    return (rbytes);
}

static ssize_t __io_v2_bytes_write(void *vctx, const void *buf, size_t nbytes)
{
    t_io_v2_bytes_ctx *ctx;

    if (NULL == vctx || NULL == buf) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }
    if (nbytes == 0) {
        IO_LOG(TRACE, "buffer size is 0");
        return (0);
    }
    ctx = (t_io_v2_bytes_ctx *)vctx;

    if (NULL == ft_ostr_append(ctx->ostring, (void *)buf, nbytes)) {
        IO_LOG(ERROR, "failed to append bytes to ostring");
        return (IO_V2_STATUS_ERROR);
    }
    ctx->seek += nbytes;

    return (nbytes);
}

static ssize_t __io_v2_bytes_close(void *vctx) {
    t_io_v2_bytes_ctx *ctx;

    if (NULL == vctx) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }
    ctx = (t_io_v2_bytes_ctx *)vctx;
    SSL_FREE(ctx);
    IO_LOG(TRACE, "bytes stream closed");

    return (IO_V2_STATUS_OK);
}
