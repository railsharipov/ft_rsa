#include <common.h>
#include <hash.h>

void	hash_sha512_224_update(t_hash *ctx, const unsigned char *mes, size_t messize)
{
	hash_sha512_update(ctx, mes, messize);
}
