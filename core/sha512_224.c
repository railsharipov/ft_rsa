#include <common.h>
#include <logger.h>
#include <digest.h>

#include "__digest.h"

static const t_sha512_word	HASH_INIT_VECT[] = {
	0x8c3d37c819544da2, 0x73e1996689dcd4d6,
	0x1dfab7ae32ff9c82, 0x679dd514582f9fcf,
	0x0f6d2b697bd44da8, 0x77e36f7304c48942,
	0x3f9d85a86a1d36c8, 0x1112e6ad91d692a1
};

void	sha512_224_init(t_hash *ctx)
{
	ft_bzero(ctx, sizeof(t_hash));
	ft_memcpy(ctx->var, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ft_memcpy(ctx->hash, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ctx->blocksize = SHA512_BLOCK_SIZE;
	ctx->hashsize = SHA512_224_HASH_SIZE;
}

void	sha512_224_update(t_hash *ctx, const unsigned char *mes, size_t messize)
{
	sha512_update(ctx, mes, messize);
}

void	sha512_224_final(t_hash *ctx)
{
	sha512_final(ctx);
}

t_transform_result sha512_224_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA512/224 transform update");
	return (__transform_update(vctx, sha512_update_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}

t_transform_result sha512_224_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA512/224 transform final");
	return (__transform_final(vctx, sha512_update_block, sha512_final_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}
