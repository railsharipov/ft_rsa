#include <io.h>
#include <libft/std.h>
#include <libft/alloc.h>

typedef struct	s_io_osbuf_ctx
{
	t_ostring	*osbuf;
	ssize_t		seek;
	int			delim;
	int			lwidth;
}				t_io_osbuf_ctx;

static ssize_t	__io_osbuf_read(void *ctx, char *buf, size_t nbytes);
static ssize_t	__io_osbuf_write(void *ctx, const char *buf, size_t nbytes);

static ssize_t __sread(t_io_osbuf_ctx *ctx, char *buf, size_t nbytes);
static ssize_t __sread_delim(t_io_osbuf_ctx *ctx, char *buf, size_t nbytes);
static ssize_t __swrite(t_io_osbuf_ctx *ctx, const char *buf, size_t nbytes);
static ssize_t __swrite_delim(t_io_osbuf_ctx *ctx, const char *buf, size_t nbytes);

int	io_v2_osbuf(t_iodes_v2 *iodes, uint32_t flags, t_ostring *osbuf)
{
	IO_LOG(TRACE, "io osbuf with flags=0x%x, osbuf=%p", flags, osbuf);
	t_io_osbuf_ctx	*ctx;

	if (NULL == iodes || NULL == osbuf) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_ALLOC(ctx, sizeof(t_io_osbuf_ctx));
	ft_bzero(iodes, sizeof(t_iodes_v2));
	iodes->ctx = ctx;
	ctx->osbuf = osbuf;

	iodes->read_f = __io_osbuf_read;
	iodes->write_f = __io_osbuf_write;

	return (SSL_OK);
}

static ssize_t	__io_osbuf_read(void *vctx, char *buf, size_t nbytes)
{
	size_t	rbytes;
	t_io_osbuf_ctx *ctx;

	IO_LOG(TRACE, "io sbuf read with vctx=%p, buf=%p, nbytes=%zu", vctx, buf, nbytes);

	ctx = (t_io_osbuf_ctx *)vctx;
	if (NULL == ctx || NULL == buf) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (NULL == ctx->osbuf || NULL == ctx->osbuf->content) {
		IO_LOG(ERROR, "osbuf is not set or not initialized");
		return (-1);
	}
	if (nbytes == 0) {
		IO_LOG(TRACE, "buffer size is 0, nothing to read");
		return (0);
	}
	if (ctx->delim) {
		IO_LOG(TRACE, "using delimiter-based read, delim=%d", ctx->delim);
		rbytes = __sread_delim(ctx, buf, nbytes);
	}
	else {
		IO_LOG(TRACE, "using read without delimiter");
		rbytes = __sread(ctx, buf, nbytes);
	}
	return (rbytes);
}

static ssize_t __sread(t_io_osbuf_ctx *ctx, char *buf, size_t nbytes)
{
	t_ostring	*osbuf;
	size_t		rbytes;

	osbuf = ctx->osbuf;
	nbytes = MIN(nbytes, MAX(0, osbuf->size - ctx->seek));
	rbytes = 0;

	IO_LOG(TRACE, "reading %zu bytes, osbuf size=%zu, seek=%zd", nbytes, osbuf->size, ctx->seek);

	while (nbytes-- > 0)
		buf[rbytes++] = osbuf->content[ctx->seek++];

	return (rbytes);
}

static ssize_t __sread_delim(t_io_osbuf_ctx *ctx, char *buf, size_t nbytes)
{
	t_ostring	*osbuf;
	size_t		rbytes;
	uint32_t	delim;

	osbuf = ctx->osbuf;
	nbytes = MIN(nbytes, MAX(0, osbuf->size - ctx->seek));
	delim = (uint32_t)ctx->delim;
	rbytes = 0;

	IO_LOG(TRACE, "reading %zu bytes, osbuf size=%zu, seek=%zd", nbytes, osbuf->size, ctx->seek);

	while ((rbytes < nbytes) && (ctx->seek < osbuf->size)) {
		buf[rbytes] = osbuf->content[ctx->seek++];
		if (*buf == delim) {
			continue ;
		}
		rbytes++;
	}

	return (rbytes);
}

static ssize_t	__io_osbuf_write(void *vctx, const char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "io sbuf write with vctx=%p, buf=%p, nbytes=%zu", vctx, buf, nbytes);
	t_io_osbuf_ctx *ctx;
	ssize_t	wbytes;

	ctx = (t_io_osbuf_ctx *)vctx;
	if (NULL == buf || NULL == ctx || NULL == ctx->osbuf) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (nbytes == 0) {
		IO_LOG(TRACE, "buffer size is 0, nothing to write");
		return (0);
	}
	if (ctx->delim) {
		IO_LOG(TRACE, "using delimiter-based write, delim=%d", ctx->delim);
		wbytes = __swrite_delim(ctx, (char *)buf, nbytes);
	}
	else {
		IO_LOG(TRACE, "using write without delimiter");
		wbytes = __swrite(ctx, (char *)buf, nbytes);
	}
	return (wbytes);
}

static ssize_t __swrite(t_io_osbuf_ctx *ctx, const char *buf, size_t nbytes)
{
	t_ostring	*osbuf;

	osbuf = ctx->osbuf;

	IO_LOG(TRACE, "writing %zd bytes to osbuf, current osbuf size=%zu, seek=%zd", nbytes, osbuf->size, ctx->seek);
	ft_ostr_append(osbuf, (char *)buf, nbytes);
	ctx->seek += nbytes;

	IO_LOG(TRACE, "wrote %zd bytes, new osbuf size=%zu, seek=%zd", nbytes, osbuf->size, ctx->seek);
	return (nbytes);
}

static ssize_t	__swrite_delim(t_io_osbuf_ctx *ctx, const char *buf, size_t nbytes)
{
	char		wbuf[2*nbytes];
	t_ostring	*osbuf;
	ssize_t		wbytes;
	ssize_t		offset;
	ssize_t		width;
	ssize_t		ix;

	osbuf = ctx->osbuf;

	if (ctx->lwidth <= 0) {
		IO_LOG(TRACE, "setting default lwidth to 64");
		ctx->lwidth = 64;
	}

	width = (size_t)ctx->lwidth;
	offset = MIN(nbytes, (ctx->seek % width));
	wbytes = 0;

	IO_LOG(TRACE, "width=%zd, offset=%zd, current osbuf size=%zu, seek=%zd", width, offset, osbuf->size, ctx->seek);

	/* Finish last line if it's not [width] bytes long */
	if (offset) {
		IO_LOG(TRACE, "finishing last line with offset");
		for (; wbytes < (width - offset); nbytes--)
			wbuf[wbytes++] = *buf++;
		if (nbytes > 0) {
			wbuf[wbytes++] = ctx->delim;
		}
	}

	IO_LOG(TRACE, "writing %zu bytes", nbytes);
	while (nbytes/width > 0) {
		for (ix = 0; ix < width; ix++, nbytes--) {
			wbuf[wbytes++] = *buf++;
		}
		wbuf[wbytes++] = ctx->delim;
	}
	while (nbytes-- > 0) {
		wbuf[wbytes++] = *buf++;
	}

	ft_ostr_append(osbuf, wbuf, wbytes);
	ctx->seek += wbytes;

	IO_LOG(TRACE, "wrote %zd bytes, new osbuf size=%zu, seek=%zd", wbytes, osbuf->size, ctx->seek);
	return (wbytes);
}