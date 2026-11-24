#include <libft_v2.h>
#include <logger.h>

static ssize_t __read_from_upstream(void *ctx, void *buf, size_t nbytes);
static ssize_t __write_to_downstream(void *ctx, const void *buf, size_t nbytes);

typedef struct s_io_v2_pipe_ctx {
	t_io_v2_stream *upstream;
	t_io_v2_stream *downstream;
	t_buffer *buffer;
} t_io_v2_pipe_ctx;

bool io_v2_pipe_unidir(t_io_v2_pipe **pipe, t_io_v2_stream *upstream, t_io_v2_stream *downstream)
{
	t_io_v2_pipe_ctx *ctx;

    if (NULL == pipe || NULL == upstream || NULL == downstream) {
        LIBFT_LOG(ERROR, IO_INPUT_ERROR);
        return false;
    }
    if (!FLAG(IO_V2_FLAG_READ, upstream->flags)) {
        LIBFT_LOG(ERROR, "upstream stream is not readable");
        return false;
    }
    if (!FLAG(IO_V2_FLAG_WRITE, downstream->flags)) {
        LIBFT_LOG(ERROR, "downstream stream is not writable");
        return false;
    }
    LIBFT_ALLOC((*pipe), sizeof(t_io_v2_pipe));
	LIBFT_ALLOC(ctx, sizeof(t_io_v2_pipe_ctx));
	ctx->upstream = upstream;
	ctx->downstream = downstream;
	ctx->buffer = ft_buffer_new(IO_BUFSIZE);
    (*pipe)->ctx = ctx;
    (*pipe)->type = IO_V2_PIPE_TYPE_UNIDIR;
    (*pipe)->status = IO_V2_STATUS_PIPE_OK;

    return true;
}

bool io_v2_pipe_bidir(t_io_v2_pipe **pipe, t_io_v2_stream *upstream, t_io_v2_stream *downstream)
{
	if (NULL == pipe || NULL == upstream || NULL == downstream) {
		LIBFT_LOG(ERROR, IO_INPUT_ERROR);
		return false;
	}
    LIBFT_LOG(ERROR, "not implemented error");

    return false;
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

bool io_v2_pipe_pump(t_io_v2_pipe *pipe)
{
	t_io_v2_pipe_ctx *ctx;
    t_buffer *buffer;

    LIBFT_LOG(TRACE, "pumping though pipe %p", pipe);

    if (NULL == pipe) {
        LIBFT_LOG(ERROR, IO_INPUT_ERROR);
        return false;
    }
    switch (pipe->status) {
        case IO_V2_STATUS_PIPE_OK:
            break;
        case IO_V2_STATUS_PIPE_ERROR:
			LIBFT_LOG(ERROR, "pipe is in error state");
			return false;
        case IO_V2_STATUS_PIPE_END:
			LIBFT_LOG(ERROR, "pipe is in end state");
			return false;
        default:
            LIBFT_LOG(ERROR, "invalid pipe state");
            return false;
    }
	ctx = (t_io_v2_pipe_ctx *)pipe->ctx;
    buffer = ctx->buffer;

    /* Write from internal buffer to downstream */
    t_io_v2_stream *downstream = ctx->downstream;
    ssize_t wbytes = 0;

    switch (downstream->status) {
    case IO_V2_STATUS_OK:
	   	LIBFT_LOG(TRACE, "writing %zu bytes max to downstream", ft_buffer_used(buffer));
		wbytes = ft_buffer_read_with_func(buffer, __write_to_downstream, downstream, ft_buffer_used(buffer));

		switch (downstream->status) {
		case IO_V2_STATUS_OK:
		case IO_V2_STATUS_FINISHED:
	   		LIBFT_LOG(TRACE, "wrote %zu bytes to downstream", wbytes);
	       	break;
		case IO_V2_STATUS_EOF:
		case IO_V2_STATUS_CLOSED:
		case IO_V2_STATUS_ERROR:
		default:
			LIBFT_LOG(ERROR, "write to downstream failed");
			pipe->status = IO_V2_STATUS_PIPE_ERROR;
			return false;
	    }
    	break;
    case IO_V2_STATUS_ERROR:
		LIBFT_LOG(ERROR, "downstream is in error state");
		pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return false;
    case IO_V2_STATUS_CLOSED:
    	LIBFT_LOG(ERROR, "downstream is in closed state");
     	pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return false;
    case IO_V2_STATUS_FINISHED:
    	LIBFT_LOG(ERROR, "downstream is in finished state");
    	pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return false;
    case IO_V2_STATUS_EOF:
    default:
  		LIBFT_LOG(ERROR, "downstream is in invalid state");
   		pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return false;
    }

   	/* Read from upstream into internal buffer */
    t_io_v2_stream *upstream = ctx->upstream;
    ssize_t rbytes = 0;

    switch (upstream->status) {
    case IO_V2_STATUS_OK:
	   	LIBFT_LOG(TRACE, "reading %zu bytes max from upstream", ft_buffer_available(buffer));
	    rbytes = ft_buffer_write_with_func(buffer, __read_from_upstream, upstream, ft_buffer_available(buffer));

	   	switch (upstream->status) {
		case IO_V2_STATUS_OK:
	   		LIBFT_LOG(TRACE, "read %zu bytes from upstream", rbytes);
	       	break;
		case IO_V2_STATUS_EOF:
			LIBFT_LOG(TRACE, "upstream is at EOF");
			break;
		case IO_V2_STATUS_CLOSED:
		case IO_V2_STATUS_FINISHED:
		case IO_V2_STATUS_ERROR:
		default:
			LIBFT_LOG(ERROR, "read from upstream failed");
			pipe->status = IO_V2_STATUS_PIPE_ERROR;
			return false;
	    }
        break;
    case IO_V2_STATUS_EOF:
    	if (ft_buffer_is_empty(buffer)) {
			LIBFT_LOG(TRACE, "upstream is at EOF and internal buffer is empty");
			pipe->status = IO_V2_STATUS_PIPE_END;
		}
     	break;
    case IO_V2_STATUS_ERROR:
		LIBFT_LOG(ERROR, "upstream is in error state");
		pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return false;
    case IO_V2_STATUS_CLOSED:
    	LIBFT_LOG(ERROR, "upstream is in closed state");
     	pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return false;
    case IO_V2_STATUS_FINISHED:
    default:
  		LIBFT_LOG(ERROR, "upstream is in invalid state");
   		pipe->status = IO_V2_STATUS_PIPE_ERROR;
        return false;
    }
    return true;
}

bool io_v2_pipe_del(t_io_v2_pipe *pipe)
{
	t_io_v2_pipe_ctx *ctx = pipe->ctx;
	bool ret = true;

	LIBFT_LOG(TRACE, "deleting pipe");
    if (NULL == pipe) {
        LIBFT_LOG(ERROR, IO_INPUT_ERROR);
        return false;
    }
	if (!ft_buffer_is_empty(ctx->buffer)) {
		LIBFT_LOG(ERROR, "deleting pipe with data loss");
		ret = false;
	}
    ft_buffer_del(ctx->buffer);
	LIBFT_FREE(ctx);
	pipe->ctx = NULL;
	pipe->status = IO_V2_STATUS_PIPE_END;
	LIBFT_LOG(TRACE, "deleted pipe");

   	return (ret);
}
