#include <io.h>
#include <logger.h>

static ssize_t __read_from_upstream(void *ctx, void *buf, size_t nbytes);
static ssize_t __write_to_downstream(void *ctx, const void *buf, size_t nbytes);

typedef struct s_io_v2_pipe_ctx {
	t_io_v2_stream *upstream;
	t_io_v2_stream *downstream;
	t_buffer *buffer;
} t_io_v2_pipe_ctx;

int io_v2_pipe_unidir(t_io_v2_pipe **pipe, t_io_v2_stream *upstream, t_io_v2_stream *downstream)
{
	t_io_v2_pipe_ctx *ctx;

    if (NULL == pipe || NULL == upstream || NULL == downstream) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    if (!SSL_FLAG(IO_V2_FLAG_READ, upstream->flags)) {
        SSL_LOG(ERROR, "upstream stream is not readable");
        return (SSL_ERR);
    }
    if (!SSL_FLAG(IO_V2_FLAG_WRITE, downstream->flags)) {
        SSL_LOG(ERROR, "downstream stream is not writable");
        return (SSL_ERR);
    }
    SSL_ALLOC((*pipe), sizeof(t_io_v2_pipe));
	SSL_ALLOC(ctx, sizeof(t_io_v2_pipe_ctx));
	ctx->upstream = upstream;
	ctx->downstream = downstream;
	ctx->buffer = ft_buffer_new(IO_BUFSIZE);
    (*pipe)->ctx = ctx;
    (*pipe)->type = IO_V2_PIPE_TYPE_UNIDIR;
    (*pipe)->status = IO_V2_STATUS_PIPE_OK;
    return (SSL_OK);
}

int io_v2_pipe_bidir(t_io_v2_pipe **pipe, t_io_v2_stream *upstream, t_io_v2_stream *downstream)
{
	if (NULL == pipe || NULL == upstream || NULL == downstream) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
    SSL_LOG(ERROR, NOT_IMPLEMENTED_ERROR);

    return (SSL_ERR);
}

static ssize_t __read_from_upstream(void *ctx, void *buf, size_t nbytes)
{
	t_io_v2_stream *upstream;

	upstream = (t_io_v2_stream *)ctx;
	return (io_v2_read(upstream, buf, nbytes));
}

static ssize_t __write_to_downstream(void *ctx, const void *buf, size_t nbytes)
{
	t_io_v2_stream *downstream;

	downstream = (t_io_v2_stream *)ctx;
	return (io_v2_write(downstream, buf, nbytes));
}

int io_v2_pipe_pump(t_io_v2_pipe *pipe)
{
	t_io_v2_pipe_ctx *ctx;
    t_buffer *buffer;

    SSL_LOG(TRACE, "pumping though pipe %p", pipe);

    if (NULL == pipe) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
    switch (pipe->status) {
        case IO_V2_STATUS_PIPE_OK:
            break;
        case IO_V2_STATUS_PIPE_ERROR:
			SSL_LOG(ERROR, "pipe is in error state");
			return (-1);
        case IO_V2_STATUS_PIPE_END:
			SSL_LOG(ERROR, "pipe is in end state");
			return (-1);
        default:
            SSL_LOG(ERROR, "invalid pipe state");
            return (-1);
    }
	ctx = (t_io_v2_pipe_ctx *)pipe->ctx;
    buffer = ctx->buffer;

    /* Write from internal buffer to downstream */
    t_io_v2_stream *downstream = ctx->downstream;
    ssize_t wbytes = 0;

    switch (downstream->status) {
    case IO_V2_STATUS_OK:
	   	SSL_LOG(TRACE, "writing %zu bytes max to downstream", ft_buffer_used(buffer));
		wbytes = ft_buffer_read_with_func(buffer, __write_to_downstream, downstream, ft_buffer_used(buffer));

		switch (downstream->status) {
		case IO_V2_STATUS_OK:
		case IO_V2_STATUS_FINISHED:
	   		SSL_LOG(TRACE, "wrote %zu bytes to downstream", wbytes);
	       	break;
		default:
			SSL_LOG(ERROR, "write to downstream failed");
			pipe->status = IO_V2_STATUS_PIPE_ERROR;
			return (-1);
	    }
    	break;
    case IO_V2_STATUS_ERROR:
		SSL_LOG(ERROR, "downstream is in error state");
		pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return (-1);
    case IO_V2_STATUS_CLOSED:
    	SSL_LOG(ERROR, "downstream is in closed state");
     	pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return (-1);
    case IO_V2_STATUS_FINISHED:
    	SSL_LOG(ERROR, "downstream is in finished state");
    	pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return (-1);
    default:
  		SSL_LOG(ERROR, "downstream is in invalid state");
   		pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return (-1);
    }

   	/* Read from upstream into internal buffer */
    t_io_v2_stream *upstream = ctx->upstream;
    ssize_t rbytes = 0;

    switch (upstream->status) {
    case IO_V2_STATUS_OK:
	   	SSL_LOG(TRACE, "reading %zu bytes max from upstream", ft_buffer_available(buffer));
	    rbytes = ft_buffer_write_with_func(buffer, __read_from_upstream, upstream, ft_buffer_available(buffer));

	   	switch (upstream->status) {
		case IO_V2_STATUS_OK:
	   		SSL_LOG(TRACE, "read %zu bytes from upstream", rbytes);
	       	break;
		case IO_V2_STATUS_EOF:
			SSL_LOG(TRACE, "upstream is at EOF");
			break;
		default:
			SSL_LOG(ERROR, "read from upstream failed");
			pipe->status = IO_V2_STATUS_PIPE_ERROR;
			return (-1);
	    }
        break;
    case IO_V2_STATUS_EOF:
    	if (ft_buffer_is_empty(buffer)) {
			SSL_LOG(TRACE, "upstream is at EOF and internal buffer is empty");
			pipe->status = IO_V2_STATUS_PIPE_END;
		}
     	break;
    case IO_V2_STATUS_ERROR:
		SSL_LOG(ERROR, "upstream is in error state");
		pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return (-1);
    case IO_V2_STATUS_CLOSED:
    	SSL_LOG(ERROR, "upstream is in closed state");
     	pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return (-1);
    default:
  		SSL_LOG(ERROR, "upstream is in invalid state");
   		pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return (-1);
    }
    return (0);
}

int io_v2_pipe_del(t_io_v2_pipe *pipe)
{
	t_io_v2_pipe_ctx *ctx = pipe->ctx;
	int ret = 0;

	SSL_LOG(TRACE, "deleting pipe");
    if (NULL == pipe) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (-1);
    }
	if (!ft_buffer_is_empty(ctx->buffer)) {
		SSL_LOG(ERROR, "deleting pipe with data loss");
		ret = -1;
	}
    ft_buffer_del(ctx->buffer);
	SSL_FREE(ctx);
	pipe->ctx = NULL;
	pipe->status = IO_V2_STATUS_PIPE_END;
	SSL_LOG(TRACE, "deleted pipe");

   	return (ret);
}
