#include <file.h>
#include <unistd.h>
#include <sys/fcntl.h>

typedef struct s_io_v2_file_ctx {
    const t_file    *file;
    ssize_t         seek;
    int             fd;
} t_io_v2_file_ctx;

static ssize_t __io_v2_file_read(t_io_v2_file_ctx *ctx, char *buf, size_t nbytes);
static ssize_t __io_v2_file_write(t_io_v2_file_ctx *ctx, const char *buf, size_t nbytes);
static ssize_t __io_v2_file_close(t_io_v2_file_ctx *ctx);

int file_reader(t_io_v2_stream **stream, const t_file *file)
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
    if (NULL == file) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    fd = open(file->path, O_RDONLY);
    if (fd < 0) {
        IO_LOG(ERROR, "failed to open file %s: %s", file->path, strerror(errno));
        return (SSL_ERR);
    }
    SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    ctx->file = file;
    ctx->fd = fd;
    ctx->seek = 0;

    SSL_ALLOC(ctx, sizeof(t_io_v2_file_ctx));
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_READ | IO_V2_FLAG_CLOSE;
    (*stream)->status = IO_V2_STATUS_OK;
    (*stream)->ctx = ctx;

    return (SSL_OK);
}

int file_writer(t_io_v2_stream **stream, const t_file *file)
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
    if (NULL == file) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    fd = open(file->path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        IO_LOG(ERROR, "failed to open file %s: %s", file->path, strerror(errno));
        return (SSL_ERR);
    }
    SSL_ALLOC(ctx, sizeof(t_io_v2_file_ctx));
    ctx->file = file;
    ctx->fd = fd;
    ctx->seek = 0;

    SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_WRITE | IO_V2_FLAG_CLOSE;
    (*stream)->status = IO_V2_STATUS_OK;
    (*stream)->ctx = ctx;

    return (SSL_OK);
}

static ssize_t __io_v2_file_read(t_io_v2_file_ctx *ctx, char *buf, size_t nbytes)
{
    ssize_t rbytes;
    int err;

    if (NULL == ctx || NULL == buf) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }
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

static ssize_t __io_v2_file_write(t_io_v2_file_ctx *ctx, const char *buf, size_t nbytes)
{
    ssize_t wbytes;
    int err;

    if (NULL == ctx || NULL == buf) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }
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

static ssize_t __io_v2_file_close(t_io_v2_file_ctx *ctx)
{
    int result, err;

    if (NULL == ctx) {
        IO_LOG(ERROR, INVALID_INPUT_ERROR);
        return (IO_V2_STATUS_ERROR);
    }
    if (ctx->fd < 0) {
        IO_LOG(DEBUG, "fd already closed or invalid");
        return (IO_V2_STATUS_OK);
    }
    IO_LOG(DEBUG, "closing fd=%d", ctx->fd);

    result = close(ctx->fd);
    err = errno;

    if (result < 0) {
        if (err == EINTR || err == EIO) {
            IO_LOG(WARN, "close error but fd is closed: %s", strerror(errno));
            ctx->fd = -1;
            return (IO_V2_STATUS_OK);
        }
        IO_LOG(ERROR, "close error: %s", strerror(err));
        return (IO_V2_STATUS_ERROR);
    }

    ctx->fd = -1;
    return (IO_V2_STATUS_OK);
}