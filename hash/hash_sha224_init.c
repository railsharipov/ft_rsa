#include <common.h>
#include <logger.h>
#include <hash.h>

static const t_sha224_word	HASH_INIT_VECT[] = {
	0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
	0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
};

void	hash_sha224_init(t_hash *ctx)
{
	ft_bzero(ctx, sizeof(t_hash));
	ft_memcpy(ctx->var, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ft_memcpy(ctx->hash, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ctx->blocksize = SHA224_BLOCK_SIZE;
	ctx->hashsize = SHA224_HASH_SIZE;
}
