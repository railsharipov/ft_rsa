#include <io.h>
#include <logger.h>

typedef enum {
	FILTER_STREAM,
	FILTER_TRANSFORM_UPDATE,
	FILTER_TRANSFORM_FINAL,
	FILTER_DONE
} t_filter_mode;

typedef struct s_io_v2_filter_ctx {
	t_io_v2_stream		*stream;
	t_buffer			*in;
	t_buffer			*out;
	t_func_transform	f_update;
	t_func_transform	f_final;
	t_filter_mode		mode;
	void 				*filter_ctx;
} t_io_v2_filter_ctx;

static ssize_t __io_v2_filter_read(void *ctx, void *buf, size_t nbytes);
static ssize_t __io_v2_filter_read_close(void *ctx);
static ssize_t __io_v2_filter_write(void *ctx, const void *buf, size_t nbytes);
static ssize_t __io_v2_filter_write_finish(void *ctx);
static ssize_t __io_v2_filter_write_close(void *ctx);
static ssize_t __io_v2_filter_write_flush(void *ctx);

static ssize_t __read_from_upstream(void *ctx, void *buf, size_t nbytes);
static ssize_t __write_to_downstream(void *ctx, const void *buf, size_t nbytes);

int io_v2_filter_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream,
	t_func_transform f_update, t_func_transform f_final, void *filter_ctx)
{
	const t_io_v2_interface interface = {
		.read = __io_v2_filter_read,
		.close = __io_v2_filter_read_close,
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
	if (NULL == f_update) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == f_final) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	SSL_ALLOC(ctx, sizeof(t_io_v2_filter_ctx));
	ctx->stream = upstream;
	ctx->f_update = f_update;
	ctx->f_final = f_final;
	ctx->filter_ctx = filter_ctx;
	ctx->mode = FILTER_STREAM;
	ctx->in = ft_buffer_new(IO_BUFSIZE);
	ctx->out = ft_buffer_new(IO_BUFSIZE);

	if (SSL_OK != io_v2_stream(stream, interface, ctx)) {
		SSL_LOG(ERROR, IO_CREATE_STREAM_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

int io_v2_filter_writer(t_io_v2_stream **stream, t_io_v2_stream *downstream,
	t_func_transform f_update, t_func_transform f_final, void *filter_ctx)
{
	const t_io_v2_interface interface = {
		.write = __io_v2_filter_write,
		.flush = __io_v2_filter_write_flush,
		.finish = __io_v2_filter_write_finish,
		.close = __io_v2_filter_write_close,
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
	if (NULL == f_update) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == f_final) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	SSL_ALLOC(ctx, sizeof(t_io_v2_filter_ctx));
	ctx->stream = downstream;
	ctx->f_update = f_update;
	ctx->f_final = f_final;
	ctx->filter_ctx = filter_ctx;
	ctx->mode = FILTER_STREAM;
	ctx->in = ft_buffer_new(IO_BUFSIZE);
	ctx->out = ft_buffer_new(IO_BUFSIZE);

	if (SSL_OK != io_v2_stream(stream, interface, ctx)) {
		SSL_LOG(ERROR, IO_CREATE_STREAM_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static ssize_t __read_from_upstream(void *vctx, void *buf, size_t bufsize)
{
	t_io_v2_stream *upstream = vctx;
	// Reads data from upstream into the internal buffer
	ssize_t ret = io_v2_read(upstream, buf, bufsize);
	if (ret < 0 && upstream->status == IO_V2_STATUS_EOF) {
		return (0);
	}
	return ret;
}

static ssize_t __write_to_downstream(void *vctx, const void *buf, size_t bufsize)
{
	t_io_v2_stream *downstream = vctx;

	// Writes data from internal buffer to downstream
	return (io_v2_write(downstream, buf, bufsize));
}

static ssize_t __io_v2_filter_read(void *vctx, void *buf, size_t nbytes)
{
	t_io_v2_filter_ctx *ctx = vctx;
	t_io_v2_stream *upstream = ctx->stream;

	t_filter_mode next_mode = FILTER_STREAM;
	t_transform_result result = {.status = TRANSFORM_ERROR};

	if (nbytes == 0) {
		return (0);
	}
	switch (ctx->mode) {
	case FILTER_STREAM:
		// Upstream -> Input buffer
		SSL_LOG(TRACE, "filter is reading data from upstream");
		switch (upstream->status) {
		case IO_V2_STATUS_OK:
			SSL_LOG(TRACE, "reading at most %zu bytes from upstream to input buffer", nbytes);
			ssize_t rbytes = ft_buffer_write_with_func(ctx->in, __read_from_upstream, upstream, ft_buffer_available(ctx->in));
			if (rbytes < 0) {
				SSL_LOG(ERROR, "failed to read data from upstream to input buffer");
				return (IO_V2_STATUS_ERROR);
			}
			SSL_LOG(TRACE, "read %zu bytes from upstream to input buffer", rbytes);
			if (rbytes < ft_buffer_available(ctx->in)) {
				// Input buffer is not full yet so keep feeding data to it.
				next_mode = FILTER_STREAM;
			} else {
				next_mode = FILTER_TRANSFORM_UPDATE;
			}
			break;
		case IO_V2_STATUS_EOF:
			SSL_LOG(TRACE, "upstream reached EOF: nothing to read into input buffer");
			next_mode = FILTER_TRANSFORM_FINAL;
			break ;
		case IO_V2_STATUS_ERROR:
			SSL_LOG(ERROR, "upstream is in error state");
			return (IO_V2_STATUS_ERROR);
		case IO_V2_STATUS_CLOSED:
			SSL_LOG(ERROR, "upstream is closed");
			return (IO_V2_STATUS_ERROR);
		default:
			SSL_LOG(ERROR, "invalid upstream status: %d", upstream->status);
			return (IO_V2_STATUS_ERROR);
		}
		break;

	case FILTER_TRANSFORM_UPDATE:
		// Input buffer -> Transform (update) -> Output buffer
		SSL_LOG(TRACE, "filter is transforming %zu bytes max", ft_buffer_used(ctx->in));
		result = ft_buffer_transform(ctx->in, ctx->out, ctx->f_update, ctx->filter_ctx);

		switch (result.status) {
		case TRANSFORM_ERROR:
			SSL_LOG(ERROR, "failed to transform data");
			return (IO_V2_STATUS_ERROR);
		case TRANSFORM_OK:
			next_mode = FILTER_TRANSFORM_UPDATE;
			break;
		case TRANSFORM_NEED_INPUT:
			SSL_LOG(TRACE, "transform needs input");
			next_mode = FILTER_STREAM;
			break;
		case TRANSFORM_NEED_OUTPUT:
			SSL_LOG(TRACE, "transform needs output");
			next_mode = FILTER_TRANSFORM_UPDATE;
			break;
		case TRANSFORM_DONE:
		default:
			SSL_LOG(ERROR, "unexpected transform status %d", result.status);
			return (IO_V2_STATUS_ERROR);
		}
		SSL_LOG(TRACE, "filter transformer consumed %zu bytes and produced %zu bytes", result.consumed, result.produced);
		break;

	case FILTER_TRANSFORM_FINAL:
		// Input buffer -> Transform (final) -> Output buffer
		SSL_LOG(TRACE, "filter is finishing reading %zu remaining bytes", ft_buffer_used(ctx->in));
		result = ft_buffer_transform(ctx->in, ctx->out, ctx->f_final, ctx->filter_ctx);

		switch (result.status) {
		case TRANSFORM_ERROR:
			SSL_LOG(ERROR, "failed to transform data");
			return (IO_V2_STATUS_ERROR);
		case TRANSFORM_OK:
			next_mode = FILTER_TRANSFORM_FINAL;
			break;
		case TRANSFORM_NEED_OUTPUT:
			SSL_LOG(TRACE, "transform needs output");
			next_mode = FILTER_TRANSFORM_FINAL;
			break;
		case TRANSFORM_DONE:
			SSL_LOG(TRACE, "transform is done");
			if (ft_buffer_used(ctx->in) > 0) {
				SSL_LOG(ERROR, "unexpected remaining data in the input buffer: data loss");
				return (IO_V2_STATUS_ERROR);
			}
			next_mode = FILTER_DONE;
			break;
		case TRANSFORM_NEED_INPUT:
		default:
			SSL_LOG(ERROR, "unexpected transform status %d", result.status);
			return (IO_V2_STATUS_ERROR);
		}
		SSL_LOG(TRACE, "filter transformer consumed %zu bytes and produced %zu bytes", result.consumed, result.produced);
		break;
	case FILTER_DONE:
		if (ft_buffer_is_empty(ctx->out)) {
			return (IO_V2_STATUS_EOF);
		}
		next_mode = FILTER_DONE;
		break;
	default:
		SSL_LOG(ERROR, "invalid filter mode: %d", ctx->mode);
		return (IO_V2_STATUS_ERROR);
	}
	ctx->mode = next_mode;

	// Output buffer -> User buffer
	ssize_t wbytes = 0;
	if (ft_buffer_used(ctx->out) > 0) {
		SSL_LOG(TRACE, "filter is writing %zu bytes of transformed data to user buffer", nbytes);
		wbytes = ft_buffer_read(ctx->out, buf, nbytes);
		if (wbytes < 0) {
			SSL_LOG(ERROR, "filter failed to write transformed data to user buffer");
			return (IO_V2_STATUS_ERROR);
		}
		SSL_LOG(TRACE, "filter wrote %zu bytes of transformed data", wbytes);
	}
	return (wbytes);
}

static ssize_t __io_v2_filter_write(void *vctx, const void *buf, size_t nbytes)
{
	t_io_v2_filter_ctx *ctx = vctx;
	t_io_v2_stream *downstream = ctx->stream;

	t_filter_mode next_mode = FILTER_STREAM;
	t_transform_result result = {.status = TRANSFORM_ERROR};

	if (nbytes == 0) {
		return (0);
	}
	// User buffer -> Input buffer
	ssize_t rbytes = 0;
	if (ft_buffer_available(ctx->in) > 0) {
		SSL_LOG(TRACE, "filter is reading %zu bytes of user data to input buffer", nbytes);
		rbytes = ft_buffer_write(ctx->in, buf, nbytes);
		if (rbytes < 0) {
			SSL_LOG(ERROR, "filter failed to read user data to input buffer");
			return (IO_V2_STATUS_ERROR);
		}
		SSL_LOG(TRACE, "filter read %zu bytes of user data", rbytes);
	}

	switch (ctx->mode) {
	case FILTER_TRANSFORM_UPDATE:
		// Input buffer -> Transform (update) -> Output buffer
		SSL_LOG(TRACE, "filter is transforming %zu bytes max", ft_buffer_used(ctx->in));
		result = ft_buffer_transform(ctx->in, ctx->out, ctx->f_update, ctx->filter_ctx);

		switch (result.status) {
		case TRANSFORM_ERROR:
			SSL_LOG(ERROR, "failed to transform data");
			return (IO_V2_STATUS_ERROR);
		case TRANSFORM_OK:
			next_mode = FILTER_TRANSFORM_UPDATE;
			break;
		case TRANSFORM_NEED_INPUT:
			SSL_LOG(TRACE, "transform needs input");
			next_mode = FILTER_STREAM;
			break;
		case TRANSFORM_NEED_OUTPUT:
			SSL_LOG(TRACE, "transform needs output");
			next_mode = FILTER_TRANSFORM_UPDATE;
			break;
		case TRANSFORM_DONE:
		default:
			SSL_LOG(ERROR, "unexpected transform status %d", result.status);
			return (IO_V2_STATUS_ERROR);
		}
		SSL_LOG(TRACE, "filter transformer consumed %zu bytes and produced %zu bytes", result.consumed, result.produced);
		break;

	case FILTER_STREAM:
		// Output buffer -> Downstream
		SSL_LOG(TRACE, "filter is writing data to downstream");
		switch (downstream->status) {
		case IO_V2_STATUS_OK:
			SSL_LOG(TRACE, "writing at most %zu bytes to downstream from output buffer", nbytes);
			ssize_t wbytes = ft_buffer_read_with_func(ctx->in, __write_to_downstream, downstream, ft_buffer_available(ctx->out));
			if (wbytes < 0) {
				SSL_LOG(ERROR, "failed to write data to downstream from output buffer");
				return (IO_V2_STATUS_ERROR);
			}
			SSL_LOG(TRACE, "wrote %zu bytes to downstream from output buffer", wbytes);
			if (ft_buffer_available(ctx->out) > 0) {
				// Output buffer is not empty yet so keep dumping data from it.
				next_mode = FILTER_STREAM;
			} else {
				next_mode = FILTER_TRANSFORM_UPDATE;
			}
			break;
		case IO_V2_STATUS_ERROR:
			SSL_LOG(ERROR, "downstream is in error state");
			return (IO_V2_STATUS_ERROR);
		case IO_V2_STATUS_CLOSED:
			SSL_LOG(ERROR, "downstream is closed");
			return (IO_V2_STATUS_ERROR);
		case IO_V2_STATUS_EOF:
		default:
			SSL_LOG(ERROR, "invalid downstream status: %d", downstream->status);
			return (IO_V2_STATUS_ERROR);
		}
		break;

	case FILTER_TRANSFORM_FINAL:
	case FILTER_DONE:
	default:
		SSL_LOG(ERROR, "invalid filter mode: %d", ctx->mode);
		return (IO_V2_STATUS_ERROR);
	}
	ctx->mode = next_mode;

	return (rbytes);
}

static ssize_t __io_v2_filter_write_finish(void *vctx)
{
	t_io_v2_filter_ctx *ctx = vctx;
	t_io_v2_stream *downstream = ctx->stream;

	t_filter_mode next_mode = FILTER_STREAM;
	t_transform_result result = {.status = TRANSFORM_ERROR};

	ssize_t tbytes = 0;
	while (ctx->mode != FILTER_DONE) {
		switch (ctx->mode) {
		case FILTER_TRANSFORM_UPDATE:
			// We are in finishing filter mode so treat as FILTER_TRANSFORM_FINAL and fall though.
		case FILTER_TRANSFORM_FINAL:
			// Input buffer -> Transform (final) -> Output buffer
			SSL_LOG(TRACE, "filter is transforming %zu bytes max", ft_buffer_used(ctx->in));
			result = ft_buffer_transform(ctx->in, ctx->out, ctx->f_final, ctx->filter_ctx);

			switch (result.status) {
			case TRANSFORM_ERROR:
				SSL_LOG(ERROR, "failed to transform data");
				return (IO_V2_STATUS_ERROR);
			case TRANSFORM_OK:
				next_mode = FILTER_TRANSFORM_FINAL;
				break;
			case TRANSFORM_NEED_OUTPUT:
				SSL_LOG(TRACE, "transform needs output");
				next_mode = FILTER_TRANSFORM_FINAL;
				break;
			case TRANSFORM_DONE:
				SSL_LOG(TRACE, "transform is done");
				if (ft_buffer_used(ctx->in) > 0) {
					SSL_LOG(ERROR, "unexpected remaining data in the input buffer: data loss");
					return (IO_V2_STATUS_ERROR);
				}
				next_mode = FILTER_DONE;
				break;
			case TRANSFORM_NEED_INPUT:
			default:
				SSL_LOG(ERROR, "unexpected transform status %d", result.status);
				return (IO_V2_STATUS_ERROR);
			}
			SSL_LOG(TRACE, "filter transformer consumed %zu bytes and produced %zu bytes", result.consumed, result.produced);
			break;

		case FILTER_STREAM:
			// Output buffer -> Downstream
			SSL_LOG(TRACE, "filter is writing data to downstream");
			switch (downstream->status) {
			case IO_V2_STATUS_OK:
				SSL_LOG(TRACE, "writing at most %zu bytes to downstream from output buffer", ft_buffer_available(ctx->out));
				ssize_t wbytes = ft_buffer_read_with_func(ctx->out, __write_to_downstream, downstream, ft_buffer_available(ctx->out));
				if (wbytes < 0) {
					SSL_LOG(ERROR, "failed to write data to downstream from output buffer");
					return (IO_V2_STATUS_ERROR);
				}
				tbytes += wbytes;
				SSL_LOG(TRACE, "wrote %zu bytes to downstream from output buffer", wbytes);
				if (ft_buffer_available(ctx->out) > 0) {
					// Output buffer is not empty yet so keep dumping data from it.
					next_mode = FILTER_STREAM;
				} else {
					next_mode = FILTER_TRANSFORM_FINAL;
				}
				break;
			case IO_V2_STATUS_ERROR:
				SSL_LOG(ERROR, "downstream is in error state");
				return (IO_V2_STATUS_ERROR);
			case IO_V2_STATUS_CLOSED:
				SSL_LOG(ERROR, "downstream is closed");
				return (IO_V2_STATUS_ERROR);
			case IO_V2_STATUS_EOF:
			default:
				SSL_LOG(ERROR, "invalid downstream status: %d", downstream->status);
				return (IO_V2_STATUS_ERROR);
			}
			break;

		case FILTER_DONE:
			break;

		default:
			SSL_LOG(ERROR, "invalid filter mode: %d", ctx->mode);
			return (IO_V2_STATUS_ERROR);
		}
		ctx->mode = next_mode;
	}
	return (tbytes);
}

static ssize_t __io_v2_filter_write_flush(void *vctx)
{
	t_io_v2_filter_ctx *ctx = vctx;
	t_io_v2_stream *downstream = ctx->stream;

	t_filter_mode next_mode = FILTER_STREAM;

	ssize_t tbytes = 0;
	while (!ft_buffer_is_empty(ctx->out)) {
		switch (ctx->mode) {
		case FILTER_TRANSFORM_UPDATE:
			SSL_LOG(TRACE, "skipping transform when flushing and switching to downstream dumping");
			next_mode = FILTER_STREAM;
			break;
		case FILTER_STREAM:
			// Output buffer -> Downstream
			SSL_LOG(TRACE, "filter is writing data to downstream");
			switch (downstream->status) {
			case IO_V2_STATUS_OK:
				SSL_LOG(TRACE, "writing at most %zu bytes to downstream from output buffer", ft_buffer_available(ctx->out));
				ssize_t wbytes = ft_buffer_read_with_func(ctx->out, __write_to_downstream, downstream, ft_buffer_available(ctx->out));
				if (wbytes < 0) {
					SSL_LOG(ERROR, "failed to write data to downstream from output buffer");
					return (IO_V2_STATUS_ERROR);
				}
				SSL_LOG(TRACE, "wrote %zu bytes to downstream from output buffer", wbytes);
				tbytes += wbytes;
				next_mode = FILTER_STREAM;
				break;
			case IO_V2_STATUS_ERROR:
				SSL_LOG(ERROR, "downstream is in error state");
				return (IO_V2_STATUS_ERROR);
			case IO_V2_STATUS_CLOSED:
				SSL_LOG(ERROR, "downstream is closed");
				return (IO_V2_STATUS_ERROR);
			case IO_V2_STATUS_EOF:
			default:
				SSL_LOG(ERROR, "invalid downstream status: %d", downstream->status);
				return (IO_V2_STATUS_ERROR);
			}
			break;
		case FILTER_TRANSFORM_FINAL:
		case FILTER_DONE:
		default:
			SSL_LOG(ERROR, "invalid filter mode: %d", ctx->mode);
			return (IO_V2_STATUS_ERROR);
		}
		ctx->mode = next_mode;
	}

	return (tbytes);
}

static ssize_t __io_v2_filter_read_close(void *vctx)
{
	t_io_v2_filter_ctx *ctx = vctx;
	int close_ret = IO_V2_STATUS_OK;

	SSL_LOG(TRACE, "closing filter stream");
	if (io_v2_close(ctx->stream) < 0) {
		SSL_LOG(ERROR, "failed to close upstream/downstream");
		close_ret = IO_V2_STATUS_ERROR;
	}
	SSL_LOG(TRACE, "closed upstream/downstream");

	ft_buffer_del(ctx->in);
	ctx->in = NULL;
	ft_buffer_del(ctx->out);
	ctx->out = NULL;
	SSL_FREE(ctx);

	return (close_ret);
}

static ssize_t __io_v2_filter_write_close(void *vctx)
{
	t_io_v2_filter_ctx *ctx = vctx;
	int finish_ret = IO_V2_STATUS_OK;
	int close_ret = IO_V2_STATUS_OK;

	SSL_LOG(TRACE, "closing filter stream");
	if (__io_v2_filter_write_finish(vctx) < 0) {
		SSL_LOG(ERROR, "failed to finish upstream/downstream");
		finish_ret = IO_V2_STATUS_ERROR;
	}
	if (io_v2_close(ctx->stream) < 0) {
		SSL_LOG(ERROR, "failed to close upstream/downstream");
		close_ret = IO_V2_STATUS_ERROR;
	}
	SSL_LOG(TRACE, "closed upstream/downstream");

	ft_buffer_del(ctx->in);
	ctx->in = NULL;
	ft_buffer_del(ctx->out);
	ctx->out = NULL;
	SSL_FREE(ctx);

	if (IO_V2_STATUS_OK == finish_ret && IO_V2_STATUS_OK == close_ret) {
		return (IO_V2_STATUS_OK);
	} else {
		return (IO_V2_STATUS_ERROR);
	}
}
