#include <common.h>
#include <logger.h>
#include <hash.h>

t_transform_result md5_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
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

	if (insize < MD5_BLOCK_SIZE) {
		SSL_LOG(TRACE, "need input");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_INPUT};
	}
	if (outsize < MD5_BLOCK_SIZE) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_OUTPUT};
	}
	while (i+MD5_BLOCK_SIZE <= insize && i+MD5_BLOCK_SIZE <= outsize) {
		md5_update_block(ctx, mesin + i);
		ft_memcpy((uint8_t *)mesout + i, mesin + i, MD5_BLOCK_SIZE);
		i += MD5_BLOCK_SIZE;
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_OK};
}

t_transform_result md5_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
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

	while (i+MD5_BLOCK_SIZE <= insize && i+MD5_BLOCK_SIZE <= outsize) {
		md5_update_block(ctx, mesin + i);
		ft_memcpy((uint8_t *)mesout + i, mesin + i, MD5_BLOCK_SIZE);
		i += MD5_BLOCK_SIZE;
	}
	if (insize <= outsize) {
		md5_final_block(ctx, mesin + i, insize - i);
		ft_memcpy((uint8_t *)mesout + i, mesin + i, insize - i);

		SSL_LOG(TRACE, "processing is complete");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_DONE};
	} else {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_OUTPUT};
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_OK};
}
