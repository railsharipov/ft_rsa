#include <io.h>
#include <unistd.h>
#include <sys/fcntl.h>

typedef struct s_io_v2_file_ctx {
    int    		fd;
    ssize_t		seek;
} t_io_v2_file_ctx;

static ssize_t __io_v2_file_read(void *vctx, void *buf, size_t nbytes);
static ssize_t __io_v2_file_write(void *vctx, const void *buf, size_t nbytes);
static ssize_t __io_v2_file_close(void *vctx);

int io_v2_file_reader(t_io_v2_stream **stream, const char *file_path)
{
    const t_io_v2_interface interface = {
        .read = __io_v2_file_read,
        .close = __io_v2_file_close,
    };
    t_io_v2_file_ctx *ctx;
    int fd;

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (NULL == file_path) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        IO_LOG(ERROR, "failed to open file %s: %s", file_path, strerror(errno));
        return (SSL_ERR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_v2_file_ctx));
    ctx->fd = fd;
    ctx->seek = 0;

    SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_READ | IO_V2_FLAG_CLOSE;
    (*stream)->status = IO_V2_STATUS_OK;
    (*stream)->ctx = ctx;

    return (SSL_OK);
}

int io_v2_file_writer(t_io_v2_stream **stream, const char *file_path)
{
    const t_io_v2_interface interface = {
        .write = __io_v2_file_write,
        .close = __io_v2_file_close,
    };
    t_io_v2_file_ctx *ctx;
    int fd;

    if (NULL == stream) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (NULL == file_path) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        IO_LOG(ERROR, "failed to open file %s: %s", file_path, strerror(errno));
        return (SSL_ERR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_v2_file_ctx));
    ctx->fd = fd;
    ctx->seek = 0;

    SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_WRITE | IO_V2_FLAG_CLOSE;
    (*stream)->status = IO_V2_STATUS_OK;
    (*stream)->ctx = ctx;

    return (SSL_OK);
}

static ssize_t __io_v2_file_read(void *vctx, void *buf, size_t nbytes)
{
    t_io_v2_file_ctx *ctx;
    ssize_t rbytes;
    int err;

    if (NULL == vctx || NULL == buf) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }

    ctx = (t_io_v2_file_ctx *)vctx;
    rbytes = read(ctx->fd, buf, nbytes);
    err = errno;

    if (rbytes > 0) {
        ctx->seek += rbytes;
        IO_LOG(TRACE, "read %zd bytes from fd=%d", rbytes, ctx->fd);
        return (rbytes);
    }
    else if (rbytes == 0) {
        IO_LOG(DEBUG, "EOF reached on fd=%d", ctx->fd);
        return (IO_V2_STATUS_EOF);
    }
    else {
        if (err == EINTR) {
            IO_LOG(DEBUG, "read interrupted by signal");
            return (0);
        }
        else if (err == EAGAIN || err == EWOULDBLOCK) {
            IO_LOG(DEBUG, "no data available (non-blocking)");
            return (0);
        }
        else {
            IO_LOG(ERROR, "read error on fd=%d: %s", ctx->fd, strerror(err));
            return (IO_V2_STATUS_ERROR);
        }
    }
}

static ssize_t __io_v2_file_write(void *vctx, const void *buf, size_t nbytes)
{
    t_io_v2_file_ctx *ctx;
    ssize_t wbytes;
    int err;

    if (NULL == vctx || NULL == buf) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }
    ctx = (t_io_v2_file_ctx *)vctx;
    wbytes = write(ctx->fd, buf, nbytes);
    err = errno;

    if (wbytes > 0) {
        ctx->seek += wbytes;
        IO_LOG(TRACE, "wrote %zd bytes to fd=%d", wbytes, ctx->fd);
        return (wbytes);
    }
    else if (wbytes == 0) {
        IO_LOG(ERROR, "write returned 0 (disk full?) on fd=%d", ctx->fd);
        return (IO_V2_STATUS_ERROR);
    }
    else {
        if (err == EINTR) {
            IO_LOG(DEBUG, "write interrupted by signal");
            return (0);
        }
        else if (err == EAGAIN || err == EWOULDBLOCK) {
            IO_LOG(DEBUG, "cannot write now (non-blocking)");
            return (0);
        }
        else {
            IO_LOG(ERROR, "write error on fd=%d: %s", ctx->fd, strerror(err));
            return (IO_V2_STATUS_ERROR);
        }
    }
}

static ssize_t __io_v2_file_close(void *vctx)
{
    t_io_v2_file_ctx *ctx;
    int result, err;

    if (NULL == vctx) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }
    ctx = (t_io_v2_file_ctx *)vctx;

    if (ctx->fd < 0) {
        IO_LOG(WARN, "invalid file descriptor");
        return (IO_V2_STATUS_OK);
    }
    IO_LOG(DEBUG, "closing fd=%d", ctx->fd);
    result = close(ctx->fd);
    err = errno;

    if (result < 0) {
        if (err == EINTR || err == EIO) {
            IO_LOG(WARN, "fd %d is already closed: %s", ctx->fd, strerror(err));
            ctx->fd = -1;
            return (IO_V2_STATUS_OK);
        }
        IO_LOG(ERROR, "close error: %s", strerror(err));
        return (IO_V2_STATUS_ERROR);
    }
    ctx->fd = -1;

    return (IO_V2_STATUS_OK);
}