#include <textutil.h>
#include <io.h>

typedef struct s_textutil_line_writer_ctx {
	t_io_v2_stream *buffered_downstream;
	size_t line_len;
	size_t seek;
} t_textutil_line_writer_ctx;

static ssize_t __textutil_line_write(void *vctx, const void *buf, size_t nbytes);
static ssize_t __textutil_line_flush(void *vctx);
static ssize_t __textutil_line_close(void *vctx);

int textutil_line_writer(t_io_v2_stream **stream, t_io_v2_stream *downstream, size_t line_len)
{
	const t_io_v2_interface interface = {
		.write = __textutil_line_write,
        .flush = __textutil_line_flush,
        .close = __textutil_line_close,
	};
	t_textutil_line_writer_ctx *ctx;
	t_io_v2_stream *buffered_downstream;
	size_t buffered_downstream_capacity;

    if (NULL == stream) {
        TEXTUTIL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (NULL == downstream) {
        TEXTUTIL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (line_len == 0) {
        TEXTUTIL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }

	buffered_downstream_capacity = MAX(line_len + 1, IO_BUFSIZE);
	if (SSL_OK != io_v2_buffered_writer(&buffered_downstream, downstream, buffered_downstream_capacity)) {
		TEXTUTIL_LOG(ERROR, "failed to create buffered stream");
		return (SSL_ERR);
	}
	SSL_ALLOC(ctx, sizeof(t_textutil_line_writer_ctx));
    ctx->buffered_downstream = buffered_downstream;
    ctx->line_len = line_len;
    ctx->seek = 0;

	SSL_ALLOC((*stream), sizeof(t_io_v2_stream));
    (*stream)->ctx = ctx;
    (*stream)->interface = interface;
    (*stream)->flags = IO_V2_FLAG_WRITE | IO_V2_FLAG_FLUSH | IO_V2_FLAG_CLOSE;
    (*stream)->status = IO_V2_STATUS_OK;

	return (SSL_OK);
}

static ssize_t __textutil_line_write(void *vctx, const void *buf, size_t nbytes)
{
    t_textutil_line_writer_ctx *ctx;
    ssize_t wbytes, tbytes, idx;
    const char *cbuf;

    ctx = (t_textutil_line_writer_ctx *)vctx;
    cbuf = (const char *)buf;

	tbytes = 0;
	idx = 0;
	while (idx < nbytes) {
		if (ctx->seek > 0 && ctx->seek % ctx->line_len == 0) {
			wbytes = io_v2_write(ctx->buffered_downstream, "\n", 1);
			if (wbytes < 0) {
				return (IO_V2_STATUS_ERROR);
			}
			tbytes += wbytes;
		}
		else {
			wbytes = io_v2_write(ctx->buffered_downstream, cbuf + idx, 1);
			if (wbytes < 0) {
				return (IO_V2_STATUS_ERROR);
			}
			idx += wbytes;
			tbytes += wbytes;
		}
	}
	ctx->seek += idx;

	return (tbytes);
}

static ssize_t __textutil_line_flush(void *vctx)
{
    t_textutil_line_writer_ctx *ctx;
    ssize_t wbytes;

    ctx = (t_textutil_line_writer_ctx *)vctx;

	wbytes = io_v2_flush(ctx->buffered_downstream);
    if (wbytes < 0) {
        return (IO_V2_STATUS_ERROR);
    }
	return (wbytes);
}

static ssize_t __textutil_line_close(void *vctx)
{
	t_textutil_line_writer_ctx *ctx;

	ctx = (t_textutil_line_writer_ctx *)vctx;

	if (io_v2_close(ctx->buffered_downstream) < 0) {
		return (IO_V2_STATUS_ERROR);
	}
	SSL_FREE(ctx);
	return (IO_V2_STATUS_OK);
}
