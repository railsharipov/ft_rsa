#include <textutil.h>
#include <io.h>

typedef struct s_textutil_line_reader_ctx {
	t_io_v2_stream *buffered_upstream;
	size_t max_line_len;
	size_t seek;
} t_textutil_line_reader_ctx;

static ssize_t __textutil_line_read(void *ctx, const char *buf, size_t nbytes);
static ssize_t __textutil_line_close(void *ctx);

int textutil_line_reader(t_io_v2_stream **stream, t_io_v2_stream *buffered_upstream, size_t max_line_len)
{
	const t_io_v2_interface interface = {
		.read = __textutil_line_read,
		.close = __textutil_line_close
	};
	t_io_v2_stream *buffered_stream;
	t_textutil_line_reader_ctx *ctx;
	size_t buffered_upstream_capacity;

    if (NULL == stream) {
        TEXTUTIL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (NULL == buffered_upstream) {
        TEXTUTIL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (max_line_len == 0) {
        TEXTUTIL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }

	buffered_upstream_capacity = MAX(max_line_len, IO_BUFSIZE);
	if (SSL_OK != io_v2_buffered_reader(&buffered_stream, buffered_upstream, buffered_upstream_capacity)) {
		TEXTUTIL_LOG(ERROR, "failed to create buffered stream");
		return (SSL_ERR);
	}
	SSL_ALLOC(ctx, sizeof(t_textutil_line_reader_ctx));
    ctx->buffered_upstream = buffered_stream;
    ctx->max_line_len = buffered_upstream_capacity;
    ctx->seek = 0;

	SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    (*stream)->ctx = ctx;
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_READ | IO_V2_FLAG_CLOSE;

	return (SSL_OK);
}

static ssize_t __textutil_line_read(void *vctx, const char *buf, size_t max_nbytes)
{
	t_textutil_line_reader_ctx *ctx;
    ssize_t rbytes, tbytes;
	size_t nbytes;

	ctx = (t_textutil_line_reader_ctx *)vctx;

	tbytes = 0;
	nbytes = MIN(ctx->max_line_len, max_nbytes);

	while (tbytes < nbytes) {
		rbytes = io_v2_read(ctx->buffered_upstream, buf + tbytes, 1);
		if (rbytes < 0) {
			if (IO_V2_STATUS_EOF == ctx->buffered_upstream->status && tbytes > 0) {
				return (tbytes);
			} else {
				return (ctx->buffered_upstream->status);
			}
		}
		if (buf[tbytes] == '\n') {
			return (tbytes);
		}
		tbytes += rbytes;
	}

	TEXTUTIL_LOG(ERROR, "line is too long");
	return (IO_V2_STATUS_ERROR);
}

static ssize_t __textutil_line_close(void *vctx)
{
	t_textutil_line_reader_ctx *ctx;

	ctx = (t_textutil_line_reader_ctx *)vctx;

	if (IO_V2_STATUS_ERROR == io_v2_close(ctx->buffered_upstream)) {
		return (IO_V2_STATUS_ERROR);
	}
	SSL_FREE(ctx);
	return (IO_V2_STATUS_OK);
}
