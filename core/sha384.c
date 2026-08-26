#include <common.h>
#include <logger.h>
#include <digest.h>

#include "__digest.h"

static const t_sha512_word	HASH_INIT_VECT[] = {
	0xcbbb9d5dc1059ed8, 0x629a292a367cd507,
	0x9159015a3070dd17, 0x152fecd8f70e5939,
	0x67332667ffc00b31, 0x8eb44a8768581511,
	0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4
};

void	sha384_init(t_hash *ctx)
{
	ft_bzero(ctx, sizeof(t_hash));
	ft_memcpy(ctx->var, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ft_memcpy(ctx->hash, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ctx->blocksize = SHA512_BLOCK_SIZE;
	ctx->hashsize = SHA384_HASH_SIZE;
}

void	sha384_update(t_hash *ctx, const unsigned char *mes, size_t messize)
{
	sha512_update(ctx, mes, messize);
}

void	sha384_final(t_hash *sha384)
{
	sha512_final(sha384);
}

t_transform_result sha384_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA384 transform update");
	return (__transform_update(vctx, sha512_update_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}

t_transform_result sha384_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA384 transform final");
	return (__transform_final(vctx, sha512_update_block, sha512_final_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}
