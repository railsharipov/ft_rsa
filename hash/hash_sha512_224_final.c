#include <common.h>
#include <hash.h>

void	hash_sha512_224_final(t_hash *ctx)
{
	hash_sha512_final(ctx);
}
