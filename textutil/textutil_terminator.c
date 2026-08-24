#include <textutil.h>
#include <libft.h>
#include <logger.h>

t_transform_result textutil_terminator_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_textutil_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
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

	while (i < insize && j < outsize) {
		textout[j++] = textin[i++];
	}
	if (j < outsize) {
		textout[j++] = ctx->delim;
		SSL_LOG(TRACE, "processing is complete");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
	}
	else {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
}
