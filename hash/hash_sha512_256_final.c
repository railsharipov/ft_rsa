#include <common.h>
#include <logger.h>
#include <hash.h>

void	hash_sha512_256_final(t_hash *ctx)
{
	hash_sha512_final(ctx);
}
