#include <textutil.h>
#include <libft.h>
#include <logger.h>

int	textutil_insert_delim(const char *in, size_t inlen, char **out, size_t *outlen, int delim, int step)
{
	char	*res;
	size_t	ix, iy;

	if (NULL == in || NULL == out || NULL == outlen || inlen == 0 || step <= 0) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	SSL_ALLOC(res, inlen * 2 + 1);

	for (ix = 0, iy = 0; ix < inlen; ix++) {
		if (ix != 0 && ix % (size_t)step == 0) {
			res[iy++] = delim;
		}
		res[iy++] = in[ix];
	}
	res[iy] = '\0';

	*out = res;
	*outlen = iy;

	return (SSL_OK);
}

static inline size_t __get_insert_pos(size_t offset, size_t line_width) {
	if (offset == line_width) {
		return (0);
	} else {
		return (line_width - offset%line_width);
	}
}

static inline size_t __get_new_offset(size_t offset, size_t line_width, size_t pos) {
	if (offset + pos > line_width) {
		return ((offset + pos) % line_width);
	} else {
		return (offset + pos);
	}
}

t_transform_result textutil_insert_delim_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_textutil_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->line_width == 0) {
		SSL_LOG(ERROR, "invalid line length: %zu", ctx->line_width);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already complete");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}
	const uint8_t *textin = in;
	uint8_t *textout = out;
	size_t i = 0;
	size_t j = 0;

	if (insize == 0) {
		SSL_LOG(TRACE, "need input");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_INPUT};
	}
	if (outsize == 0) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
	size_t insert_pos = __get_insert_pos(ctx->offset, ctx->line_width);

	while (i < insize && j < outsize) {
		if (i == insert_pos) {
			textout[j++] = ctx->delim;
			insert_pos = i + ctx->line_width;
		}
		if (j < outsize) {
			textout[j++] = textin[i++];
		}
	}
	ctx->offset = __get_new_offset(ctx->offset, ctx->line_width, i);
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_OK};
}

t_transform_result textutil_insert_delim_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_textutil_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->line_width == 0) {
		SSL_LOG(ERROR, "invalid line length: %zu", ctx->line_width);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already complete");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *textin = in;
	uint8_t *textout = out;
	size_t i = 0;
	size_t j = 0;

	size_t insert_pos = __get_insert_pos(ctx->offset, ctx->line_width);

	while (i < insize && j < outsize) {
		if (i == insert_pos) {
			textout[j++] = ctx->delim;
			insert_pos = i + ctx->line_width;
		}
		if (j < outsize) {
			textout[j++] = textin[i++];
		}
	}
	ctx->offset = __get_new_offset(ctx->offset, ctx->line_width, i);

	if (i >= insize) {
		SSL_LOG(TRACE, "processing is complete");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
	}
	else {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
}
