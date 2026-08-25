#include <common.h>
#include <logger.h>
#include <hash.h>

typedef void (__t_func_update)(t_hash *ctx, const uint8_t *mesblock);
typedef void (__t_func_final)(t_hash *ctx, const uint8_t *mesblock, size_t messsize);

static t_transform_result __transform_update(void *vctx, __t_func_update f_update, size_t blocksize,
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

t_transform_result md5_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running MD5 transform update");
	return (__transform_update(vctx, md5_update_block, MD5_BLOCK_SIZE, in, insize, out, outsize));
}

t_transform_result md5_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running MD5 transform final");
	return (__transform_final(vctx, md5_update_block, md5_final_block, MD5_BLOCK_SIZE, in, insize, out, outsize));
}

t_transform_result sha1_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA1 transform update");
	return (__transform_update(vctx, sha1_update_block, SHA1_BLOCK_SIZE, in, insize, out, outsize));
}

t_transform_result sha1_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA1 transform final");
	return (__transform_final(vctx, sha1_update_block, sha1_final_block, SHA1_BLOCK_SIZE, in, insize, out, outsize));
}

t_transform_result sha256_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA256 transform update");
	return (__transform_update(vctx, sha256_update_block, SHA256_BLOCK_SIZE, in, insize, out, outsize));
}

t_transform_result sha256_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA256 transform final");
	return (__transform_final(vctx, sha256_update_block, sha256_final_block, SHA256_BLOCK_SIZE, in, insize, out, outsize));
}

t_transform_result sha512_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA512 transform update");
	return (__transform_update(vctx, sha512_update_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}

t_transform_result sha512_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA512 transform final");
	return (__transform_final(vctx, sha512_update_block, sha512_final_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}
