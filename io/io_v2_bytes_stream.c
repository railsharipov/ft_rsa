#include <io.h>
#include <logger.h>

typedef struct s_io_v2_bytes_ctx {
    t_ostring	*ostring;
    size_t      seek;
} t_io_v2_bytes_ctx;

static ssize_t __io_v2_bytes_read(void *vctx, void *buf, size_t nbytes);
static ssize_t __io_v2_bytes_write(void *vctx, const void *buf, size_t nbytes);
static ssize_t __io_v2_bytes_close(void *vctx);

bool io_v2_bytes_reader(t_io_v2_stream **stream, t_ostring *ostring)
{
    const t_io_v2_interface interface = {
        .read = __io_v2_bytes_read,
        .close = __io_v2_bytes_close,
    };
    t_io_v2_bytes_ctx *ctx;

    if (NULL == stream) {
        LIBFT_LOG(ERROR, INVALID_INPUT_ERROR);
        return false;
    }
    if (NULL == ostring) {
        LIBFT_LOG(ERROR, INVALID_INPUT_ERROR);
        return false;
    }
    LIBFT_ALLOC(ctx, sizeof(t_io_v2_bytes_ctx));
    ctx->ostring = ostring;
    ctx->seek = 0;

	if (!io_v2_stream(stream, interface, ctx)) {
		LIBFT_LOG(ERROR, IO_CREATE_STREAM_ERROR);
		return false;
	}
    return true;
}

bool io_v2_bytes_writer(t_io_v2_stream **stream, t_ostring *ostring)
{
    const t_io_v2_interface interface = {
        .write = __io_v2_bytes_write,
        .close = __io_v2_bytes_close,
    };
    t_io_v2_bytes_ctx *ctx;

    if (NULL == stream) {
        LIBFT_LOG(ERROR, INVALID_INPUT_ERROR);
        return false;
    }
    if (NULL == ostring) {
        LIBFT_LOG(ERROR, INVALID_INPUT_ERROR);
        return false;
    }
    LIBFT_ALLOC(ctx, sizeof(t_io_v2_bytes_ctx));
    ctx->ostring = ostring;
    ctx->seek = 0;

	if (!io_v2_stream(stream, interface, ctx)) {
		LIBFT_LOG(ERROR, IO_CREATE_STREAM_ERROR);
		return false;
	}
    return true;
}

static ssize_t __io_v2_bytes_read(void *vctx, void *buf, size_t nbytes)
{
	t_io_v2_bytes_ctx *ctx;
    ssize_t rbytes;

    LIBFT_LOG(TRACE, "reading %zu bytes from bytes stream", nbytes);

    if (NULL == vctx || NULL == buf) {
        LIBFT_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }
    if (nbytes == 0) {
		LIBFT_LOG(TRACE, "buffer size is 0");
        return (0);
    }
	ctx = (t_io_v2_bytes_ctx *)vctx;

    if (ctx->seek >= ctx->ostring->size) {
        LIBFT_LOG(TRACE, "EOF reached");
        return (IO_V2_STATUS_EOF);
    }
    rbytes = MIN(nbytes, ctx->ostring->size - ctx->seek);
    ft_memcpy(buf, ctx->ostring->content + ctx->seek, rbytes);
    ctx->seek += rbytes;

    LIBFT_LOG(TRACE, "read %zu bytes from bytes stream", rbytes);

    return (rbytes);
}

static ssize_t __io_v2_bytes_write(void *vctx, const void *buf, size_t nbytes)
{
    t_io_v2_bytes_ctx *ctx;

	LIBFT_LOG(TRACE, "writing %zu bytes to bytes stream", nbytes);

    if (NULL == vctx || NULL == buf) {
        LIBFT_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }
    if (nbytes == 0) {
        LIBFT_LOG(TRACE, "buffer size is 0");
        return (0);
    }
    ctx = (t_io_v2_bytes_ctx *)vctx;

    if (NULL == ft_ostr_append(ctx->ostring, (void *)buf, nbytes)) {
        LIBFT_LOG(ERROR, "failed to append bytes to ostring");
        return (IO_V2_STATUS_ERROR);
    }
    ctx->seek += nbytes;

    LIBFT_LOG(TRACE, "wrote %zu bytes to bytes stream", nbytes);

    return (nbytes);
}

static ssize_t __io_v2_bytes_close(void *vctx)
{
    t_io_v2_bytes_ctx *ctx;

    LIBFT_LOG(TRACE, "closing bytes stream");

    if (NULL == vctx) {
        LIBFT_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }
    ctx = (t_io_v2_bytes_ctx *)vctx;
    LIBFT_FREE(ctx);
    LIBFT_LOG(TRACE, "bytes stream closed");

    return (IO_V2_STATUS_OK);
}
