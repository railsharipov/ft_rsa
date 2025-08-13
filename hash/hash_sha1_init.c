#include <common.h>
#include <hash.h>

static const t_sha1_word	HASH_INIT_VECT[] = {
	0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0
};

void	hash_sha1_init(t_hash *ctx)
{
	ft_bzero(ctx, sizeof(t_hash));
	ft_memcpy(ctx->var, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ft_memcpy(ctx->hash, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ctx->blocksize = SHA1_BLOCK_SIZE;
	ctx->hashsize = SHA1_HASH_SIZE;
}
