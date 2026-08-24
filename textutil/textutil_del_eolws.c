#include <textutil.h>
#include <libft.h>
#include <logger.h>

// Remove wspace in character array
// wspace = LF / CR / SP / HT / VT / FF

int textutil_del_eolws(const char *in, size_t inlen, char **out, size_t *outlen)
{
	char	*res;
	size_t   ix, iy;

	if (NULL == in || NULL == out || NULL == outlen) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_ALLOC(res, inlen + 1);
	ix = 0;
	iy = 0;
	while (ix < inlen) {
		if (!ft_iseolws(in[ix])) {
			res[iy] = in[ix];
			iy++;
		}
		ix++;
	}
	res[iy] = '\0';
	*out = res;
	*outlen = iy;

	return (SSL_OK);
}

t_transform_result textutil_del_eolws_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_textutil_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out) {
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

	if (insize == 0) {
		SSL_LOG(TRACE, "need input");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_INPUT};
	}
	while (i < insize && j < outsize) {
		if (ft_iseolws(textin[i])) {
			i++;
		} else {
			if (j < outsize) {
				textout[j++] = textin[i++];
			} else {
				break;
			}
		}
	}
	if (i <= insize) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_OK};
}

t_transform_result textutil_del_eolws_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_textutil_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out) {
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

	if (insize == 0) {
		SSL_LOG(TRACE, "no input, done");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
	}
	while (i < insize) {
		if (ft_iseolws(textin[i])) {
			i++;
		} else {
			if (j < outsize) {
				textout[j++] = textin[i++];
			} else {
				break;
			}
		}
	}
	if (i <= insize) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	} else {
		SSL_LOG(TRACE, "processing is complete");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
	}
}
