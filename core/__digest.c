#include "__digest.h"

t_transform_result __transform_update(void *vctx, __t_func_update f_update, size_t blocksize,
	const void *in, size_t insize, void *out, size_t outsize)
{
	t_hash *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "digest is already done");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *mesin = in;
	const uint8_t *mesout = in;
	size_t i = 0;

	if (insize < blocksize) {
		SSL_LOG(TRACE, "need input");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_INPUT};
	}
	if (outsize < blocksize) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_OUTPUT};
	}
	while (i+blocksize <= insize && i+blocksize <= outsize) {
		f_update(ctx, mesin + i);
		ft_memcpy((uint8_t *)mesout + i, mesin + i, blocksize);
		i += blocksize;
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_OK};
}

t_transform_result __transform_final(void *vctx, __t_func_update f_update, __t_func_final f_final, size_t blocksize,
	const void *in, size_t insize, void *out, size_t outsize)
{
	t_hash *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "digest is already done");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *mesin = in;
	const uint8_t *mesout = out;
	size_t i = 0;

	while (i+blocksize <= insize && i+blocksize <= outsize) {
		f_update(ctx, mesin + i);
		ft_memcpy((uint8_t *)mesout + i, mesin + i, blocksize);
		i += blocksize;
	}
	if (insize <= outsize) {
		f_final(ctx, mesin + i, insize - i);
		ft_memcpy((uint8_t *)mesout + i, mesin + i, insize - i);
		i = insize;
		ctx->done = 1;
		SSL_LOG(TRACE, "processing is complete");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_DONE};
	} else {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_OUTPUT};
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_OK};
}
