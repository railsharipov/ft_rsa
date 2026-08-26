#include <common.h>
#include <logger.h>
#include <digest.h>

#include "__digest.h"

static const t_sha256_word	HASH_INIT_VECT[] = {
	0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
	0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
};

void	sha224_init(t_hash *ctx)
{
	ft_bzero(ctx, sizeof(t_hash));
	ft_memcpy(ctx->var, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ft_memcpy(ctx->hash, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ctx->blocksize = SHA256_BLOCK_SIZE;
	ctx->hashsize = SHA224_HASH_SIZE;
}

void	sha224_update(t_hash *ctx, const unsigned char *mes, size_t messize)
{
	sha256_update(ctx, mes, messize);
}

void	sha224_final(t_hash *ctx)
{
	sha256_final(ctx);
}

t_transform_result sha224_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA224 transform update");
	return (__transform_update(vctx, sha256_update_block, SHA256_BLOCK_SIZE, in, insize, out, outsize));
}

t_transform_result sha224_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA256 transform final");
	return (__transform_final(vctx, sha256_update_block, sha256_final_block, SHA256_BLOCK_SIZE, in, insize, out, outsize));
}
