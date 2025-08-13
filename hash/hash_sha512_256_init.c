#include <common.h>
#include <hash.h>

static const t_sha512_256_word	HASH_INIT_VECT[] = {
	0x22312194fc2bf72c, 0x9f555fa3c84c64c2,
	0x2393b86b6f53b151, 0x963877195940eabd,
	0x96283ee2a88effe3, 0xbe5e1e2553863992,
	0x2b0199fc2c85b8aa, 0x0eb72ddc81c52ca2
};

void	hash_sha512_256_init(t_hash *ctx)
{
	ft_bzero(ctx, sizeof(t_hash));
	ft_memcpy(ctx->var, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ft_memcpy(ctx->hash, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ctx->blocksize = SHA512_BLOCK_SIZE;
	ctx->hashsize = SHA512_256_HASH_SIZE;
}
