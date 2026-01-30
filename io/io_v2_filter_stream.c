// #include <io.h>

// typedef struct s_io_v2_filter_ctx {
// 	t_io_v2_stream		*stream;
// 	t_func_io_v2_filter	filter;
// 	t_buffer			*buffer;
// 	void				*ctx;
// } t_io_v2_filter_ctx;

// static ssize_t __io_v2_filter_read(void *ctx, void *buf, size_t nbytes);
// static ssize_t __io_v2_filter_close(void *ctx);

// static ssize_t __io_v2_read_adapter(void *ctx, void *buf, size_t nbytes)
// {
// 	t_io_v2_stream *upstream;

// 	upstream = (t_io_v2_stream *)ctx;
// 	return (io_v2_read(upstream, buf, nbytes));
// }

// static ssize_t __io_v2_write_adapter(void *ctx, const void *buf, size_t nbytes)
// {
// 	t_io_v2_stream *downstream;

// 	downstream = (t_io_v2_stream *)ctx;
// 	return (io_v2_write(downstream, buf, nbytes));
// }

// int io_v2_filter_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream, t_func_io_v2_filter filter, void *filter_ctx)
// {
// 	const t_io_v2_interface interface = {
// 		.read = __io_v2_filter_read,
// 		.close = __io_v2_filter_close,
// 	};
// 	t_io_v2_filter_ctx *ctx;

// 	if (NULL == stream) {
// 		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
// 		return (SSL_ERR);
// 	}
// 	if (NULL == upstream) {
// 		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
// 		return (SSL_ERR);
// 	}
// 	SSL_ALLOC(ctx, sizeof(t_io_v2_filter_ctx));
// 	ctx->stream = upstream;
// 	ctx->filter = filter;
// 	ctx->ctx = filter_ctx;
// 	ctx->buffer = ft_buffer_new(IO_BUFSIZE);

// 	if (SSL_OK != io_v2_stream(stream, interface, (IO_V2_FLAG_READ | IO_V2_FLAG_CLOSE), ctx)) {
// 		SSL_LOG(ERROR, IO_CREATE_STREAM_ERROR);
// 		return (SSL_ERR);
// 	}
// 	return (SSL_OK);
// }

// static ssize_t __io_v2_filter_read(void *vctx, void *buf, size_t nbytes)
// {
// 	t_io_v2_filter_ctx *ctx;
// 	t_io_v2_stream *upstream;
// 	ssize_t result;
// 	ssize_t rbytes, wbytes;

// 	ctx = (t_io_v2_filter_ctx *)vctx;
// 	upstream = ctx->stream;

// 	result = ft_buffer_write_with_func(ctx->buffer, __io_v2_read_adapter, upstream, nbytes);
// 	if (result < 0) {
// 		if (upstream->status == IO_V2_STATUS_EOF) {
// 			SSL_LOG(TRACE, "upstream reached EOF");
// 			return (IO_V2_STATUS_EOF);
// 		} else {
// 			SSL_LOG(ERROR, "read from upstream failed");
// 			return (IO_V2_STATUS_ERROR);
// 		}
// 	}
// 	SSL_LOG(TRACE, "read %zu bytes from upstream", result);
// 	rbytes = result;

// 	// implement buffer view (char *vbuf, size_t vsize)
// 	// implement buffer advance/backtrack read position
// 	// implement buffer advance/backtrack write position
// 	// begin loop
// 	// while tbytes < nbytes:
// 		// if buffer is empty:
// 			// read into buffer from upstream
// 		// else:
// 			// read from buffer into vbuf
// 			// call filter function filter(vbuf, vsize, buf, nbytes, &rbytes, &wbytes)
// 			// advance buffer read position by rbytes
// 			// tbytes += rbytes
// 	// end loop
// 	// return tbytes

// 	return (wbytes);
// }