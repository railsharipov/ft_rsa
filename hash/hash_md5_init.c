#include <common.h>
#include <hash.h>
#include <libft/bytes.h>

static const t_md5_word	HASH_INIT_VECT[] = {
	0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476
};

void	hash_md5_init(t_hash *ctx)
{
	ft_bzero(ctx, sizeof(t_hash));
	ft_memcpy(ctx->var, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ft_memcpy(ctx->hash, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ctx->blocksize = MD5_BLOCK_SIZE;
	ctx->hashsize = MD5_HASH_SIZE;
}