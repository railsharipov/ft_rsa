#include <io.h>

typedef struct s_io_v2_filter_ctx {
	t_io_v2_stream		*stream;
	t_buffer			*buffer;
	t_func_buffer_transform	filter;
	void 				*filter_ctx;
} t_io_v2_filter_ctx;

static ssize_t __io_v2_filter_read(void *ctx, void *buf, size_t nbytes);
static ssize_t __io_v2_filter_write(void *ctx, const void *buf, size_t nbytes);
static ssize_t __io_v2_filter_flush(void *ctx);
static ssize_t __io_v2_filter_close(void *ctx);

static ssize_t __read_from_upstream(void *ctx, void *buf, size_t nbytes);
static ssize_t __write_to_downstream(void *ctx, const void *buf, size_t nbytes);

int io_v2_filter_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream, t_func_buffer_transform filter, void *filter_ctx)
{
	const t_io_v2_interface interface = {
		.read = __io_v2_filter_read,
		.close = __io_v2_filter_close,
	};
	t_io_v2_filter_ctx *ctx;

	if (NULL == stream) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == upstream) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_ALLOC(ctx, sizeof(t_io_v2_filter_ctx));
	ctx->stream = upstream;
	ctx->filter = filter;
	ctx->filter_ctx = filter_ctx;
	ctx->buffer = ft_buffer_new(IO_BUFSIZE);

	if (SSL_OK != io_v2_stream(stream, interface, ctx)) {
		SSL_LOG(ERROR, IO_CREATE_STREAM_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

int io_v2_filter_writer(t_io_v2_stream **stream, t_io_v2_stream *downstream, t_func_buffer_transform filter, void *filter_ctx)
{
	const t_io_v2_interface interface = {
		.write = __io_v2_filter_write,
		.flush = __io_v2_filter_flush,
		.close = __io_v2_filter_close,
	};
	t_io_v2_filter_ctx *ctx;

	if (NULL == stream) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == downstream) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_ALLOC(ctx, sizeof(t_io_v2_filter_ctx));
	ctx->stream = downstream;
	ctx->filter = filter;
	ctx->filter_ctx = filter_ctx;
	ctx->buffer = ft_buffer_new(IO_BUFSIZE);

	if (SSL_OK != io_v2_stream(stream, interface, ctx)) {
		SSL_LOG(ERROR, IO_CREATE_STREAM_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static ssize_t __read_from_upstream(void *vctx, void *buf, size_t bufsize)
{
	t_io_v2_stream *upstream;
	ssize_t		ret;

	upstream = (t_io_v2_stream *)vctx;
	// Reads data from upstream into the internal buffer
	ret = io_v2_read(upstream, buf, bufsize);
	return ((ret < 0) ? -1 : ret);
}

static ssize_t __write_to_downstream(void *vctx, const void *buf, size_t bufsize)
{
	t_io_v2_stream *downstream;
	ssize_t		ret;

	downstream = (t_io_v2_stream *)vctx;
	// Writes data from internal buffer to downstream
	ret = io_v2_write(downstream, buf, bufsize);
	return ((ret < 0) ? -1 : ret);
}

static ssize_t __io_v2_filter_read(void *vctx, void *buf, size_t nbytes)
{
	t_io_v2_filter_ctx *ctx;
	t_io_v2_stream *upstream;
	size_t consumed, produced;
	ssize_t wbytes;

	ctx = (t_io_v2_filter_ctx *)vctx;
	upstream = ctx->stream;

	// Fill internal buffer with raw data from upstream
	switch (upstream->status) {
		case IO_V2_STATUS_OK:
			SSL_LOG(TRACE, "reading at most %zu bytes from upstream to internal buffer", nbytes);
			wbytes = ft_buffer_write_with_func(ctx->buffer, __read_from_upstream, upstream, nbytes - ft_buffer_used(ctx->buffer));
			if (wbytes < 0) {
				if (upstream->status == IO_V2_STATUS_EOF) {
					wbytes = 0;
				} else {
					SSL_LOG(ERROR, "failed to read from upstream to internal buffer");
					return (IO_V2_STATUS_ERROR);
				}
			}
			SSL_LOG(TRACE, "read %zu bytes from upstream to internal buffer", wbytes);
			break;
		case IO_V2_STATUS_EOF:
			SSL_LOG(TRACE, "upstream reached EOF: nothing to read into internal buffer");
			break ;
		case IO_V2_STATUS_ERROR:
			SSL_LOG(ERROR, "upstream is in error state");
			return (IO_V2_STATUS_ERROR);
		case IO_V2_STATUS_CLOSED:
			SSL_LOG(ERROR, "upstream is closed");
			return (IO_V2_STATUS_ERROR);
		default:
			SSL_LOG(ERROR, "invalid stream status");
			return (IO_V2_STATUS_ERROR);
	}

	// Filter data from internal buffer to user buffer
	SSL_LOG(TRACE, "filtering at most %zu bytes from internal buffer with %zu bytes available", nbytes, ft_buffer_used(ctx->buffer));
	if (ft_buffer_transform_read(ctx->buffer, ctx->filter, ctx->filter_ctx, buf, nbytes, &consumed, &produced) < 0) {
		SSL_LOG(ERROR, "failed to filter data from internal buffer");
		return (IO_V2_STATUS_ERROR);
	}
	SSL_LOG(TRACE, "filter consumed %zu bytes and produced %zu bytes", consumed, produced);

	if (consumed == 0) {
		if (upstream->status == IO_V2_STATUS_EOF) {
			// filter is blocking so if data was not consumed it will never be consumed at this point
			// users are responsible for handling remaining data at EOF
			SSL_LOG(ERROR, "internal buffer data will be lost since it can not be consumed");
			return (IO_V2_STATUS_EOF);
		}
	}
	return (produced);
}

static ssize_t __io_v2_filter_write(void *vctx, const void *buf, size_t nbytes)
{
	t_io_v2_filter_ctx *ctx;
	t_io_v2_stream *downstream;
	size_t consumed, produced;
	ssize_t wbytes;

	ctx = (t_io_v2_filter_ctx *)vctx;
	downstream = ctx->stream;

	// Filter input data into internal buffer
	if (ft_buffer_used(ctx->buffer) < nbytes) {
		SSL_LOG(TRACE, "filtering at most %zu bytes from user buffer to internal buffer", nbytes);

		if (ft_buffer_transform_write(ctx->buffer, ctx->filter, ctx->filter_ctx, buf, nbytes, &consumed, &produced) < 0) {
			SSL_LOG(ERROR, "failed to filter data from user buffer to internal buffer");
			return (IO_V2_STATUS_ERROR);
		}
		SSL_LOG(TRACE, "filtered %zu bytes (consumed) and produced %zu bytes to internal buffer", consumed, produced);
	}

	// Write filtered data from internal buffer to downstream
	if (!ft_buffer_is_empty(ctx->buffer)) {
		switch (downstream->status) {
			case IO_V2_STATUS_OK:
				wbytes = ft_buffer_read_with_func(ctx->buffer, __write_to_downstream, downstream, nbytes);
				if (wbytes < 0) {
					SSL_LOG(ERROR, "failed to write from internal buffer to downstream");
					return (IO_V2_STATUS_ERROR);
				}
				SSL_LOG(TRACE, "wrote %zu bytes from internal buffer to downstream", wbytes);
				return (wbytes);
			case IO_V2_STATUS_ERROR:
				SSL_LOG(ERROR, "downstream is in error state");
				return (IO_V2_STATUS_ERROR);
			case IO_V2_STATUS_CLOSED:
				SSL_LOG(ERROR, "downstream is closed");
				return (IO_V2_STATUS_ERROR);
			default:
				SSL_LOG(ERROR, "invalid stream status");
				return (IO_V2_STATUS_ERROR);
		}
	}
	else {
		SSL_LOG(TRACE, "internal buffer is empty, nothing to write to downstream");
		return (0);
	}
}

static ssize_t __io_v2_filter_flush(void *vctx)
{
	t_io_v2_filter_ctx *ctx;
	t_io_v2_stream *downstream;
	ssize_t wbytes, tbytes;

	SSL_LOG(TRACE, "flushing filter stream");

	ctx = (t_io_v2_filter_ctx *)vctx;
	downstream = ctx->stream;

	tbytes = 0;
	while (!ft_buffer_is_empty(ctx->buffer)) {
		SSL_LOG(TRACE, "writing %zu bytes from internal buffer to downstream", ft_buffer_used(ctx->buffer));
		wbytes = ft_buffer_read_with_func(ctx->buffer, __write_to_downstream, downstream, ft_buffer_used(ctx->buffer));
		if (wbytes < 0) {
			SSL_LOG(ERROR, "failed to write from internal buffer to downstream");
			return (IO_V2_STATUS_ERROR);
		}
		SSL_LOG(TRACE, "wrote %zu bytes from internal buffer to downstream", wbytes);
		tbytes += wbytes;
	}
	SSL_LOG(TRACE, "flushed %zu bytes from internal buffer to downstream", tbytes);

	return (tbytes);
}

static ssize_t __io_v2_filter_close(void *vctx)
{
	t_io_v2_filter_ctx *ctx;

	ctx = (t_io_v2_filter_ctx *)vctx;

	SSL_LOG(TRACE, "closing filter stream");

	if (io_v2_close(ctx->stream) < 0) {
		SSL_LOG(ERROR, "failed to close wrapped stream");
		return (IO_V2_STATUS_ERROR);
	}
	SSL_LOG(TRACE, "closed wrapped stream");

	ft_buffer_del(ctx->buffer);
	ctx->buffer = NULL;
	SSL_FREE(ctx);

	SSL_LOG(TRACE, "filter stream closed");

	return (IO_V2_STATUS_OK);
}
