#include <common.h>
#include <io.h>
#include <libft/htable.h>

typedef struct	s_io_pipe_ctx
{
	t_iodes_v2	io_osbuf;
}				t_io_pipe_ctx;

static ssize_t	__io_pipe_read(void *vctx, char *buf, size_t nbytes);
static ssize_t	__io_pipe_write(void *vctx, const char *buf, size_t nbytes);

int	io_v2_pipe(t_iodes_v2 *iodes, uint32_t flags)
{
    t_io_pipe_ctx	*ctx;

    IO_LOG(TRACE, "io v2 pipe with flags=0x%x", flags);
    if (NULL == iodes) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_pipe_ctx));
    if (SSL_OK != io_v2_osbuf(&ctx->io_osbuf, flags, NULL)) {
        IO_LOG(ERROR, "failed to initialize io buffer");
        return (SSL_ERR);
    }
    ft_bzero(iodes, sizeof(t_iodes_v2));
    iodes->ctx = ctx;
    iodes->read_f = __io_pipe_read;
    iodes->write_f = __io_pipe_write;

    return (SSL_OK);
}

static ssize_t	__io_pipe_read(void *vctx, char *buf, size_t nbytes)
{
    t_io_pipe_ctx *ctx;

    ctx = (t_io_pipe_ctx *)vctx;
    if (NULL == ctx) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    return (ctx->io_osbuf.read_f(ctx->io_osbuf.ctx, buf, nbytes));
}

static ssize_t	__io_pipe_write(void *vctx, const char *buf, size_t nbytes)
{
    t_io_pipe_ctx *ctx;

    ctx = (t_io_pipe_ctx *)vctx;
    if (NULL == ctx) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    return (ctx->io_osbuf.write_f(ctx->io_osbuf.ctx, buf, nbytes));
}